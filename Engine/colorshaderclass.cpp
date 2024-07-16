#include "colorshaderclass.hpp"

ColorShaderClass::ColorShaderClass(ID3D11Device* device, HWND hwnd)
{
	wchar_t vsFilename[128];
	int error = wcscpy_s(vsFilename, 128, L"../Engine/color.vs");	// Set the filename of the vertex shader.
	if (error != 0) { return; }

	wchar_t psFilename[128];
	error = wcscpy_s(psFilename, 128, L"../Engine/color.ps");	// Set the filename of the pixel shader.
	if (error != 0) { return; }

	isInitialized = InitializeShader(device, hwnd, vsFilename, psFilename);
}

ColorShaderClass::~ColorShaderClass() {
	if (matrixBuffer) {
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout) {
		layout->Release();
		layout = 0;
	}
	if (pixelShader) {
		pixelShader->Release();
		pixelShader = 0;
	}
	if (vertexShader) {
		vertexShader->Release();
		vertexShader = 0;
	}
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix); // Set the shader parameters that it will use for rendering.
	if (result) { RenderShader(deviceContext, indexCount); }
	return result;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	bool success = SetVertexBuffer(device, hwnd, vsFilename);
	if (not success) { return false; }
	success = SetPixelBuffer(device, hwnd, psFilename);
	if (not success) { return false; }

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	auto matrixBufferDesc = SetMatrixBuffer();
	HRESULT result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	return !FAILED(result);
}

bool ColorShaderClass::SetVertexBuffer(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename) {
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	HRESULT result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result)) { return false; }
	
	result = VertexInputLayout(device, vertexShaderBuffer);
	if (FAILED(result)) { return false; }

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	return true;
}

bool ColorShaderClass::SetPixelBuffer(ID3D11Device* device, HWND hwnd, WCHAR* psFilename) {
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* pixelShaderBuffer = 0;

	HRESULT result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result)) { return false; }

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	return true;
}

HRESULT ColorShaderClass::VertexInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer){
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	auto pLayout_position = SetPolygon("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	auto pLayout_color = SetPolygon("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2] = { pLayout_position, pLayout_color };
	unsigned int numElements = sizeof(polygonLayout) / sizeof(pLayout_position);

	return device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &layout);
}

D3D11_INPUT_ELEMENT_DESC ColorShaderClass::SetPolygon(LPCSTR name, UINT index, DXGI_FORMAT format, UINT inputSlot, UINT offset, D3D11_INPUT_CLASSIFICATION slotClass, UINT stepRate) {
	D3D11_INPUT_ELEMENT_DESC p;
	p.SemanticName = name;
	p.SemanticIndex = index;
	p.Format = format;
	p.InputSlot = inputSlot;
	p.AlignedByteOffset = offset;
	p.InputSlotClass = slotClass;
	p.InstanceDataStepRate = stepRate;
	return p;
}

D3D11_BUFFER_DESC ColorShaderClass::SetMatrixBuffer() {
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc{};
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	return matrixBufferDesc;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {
	if(!errorMessage) { 
		MessageBox(hwnd, shaderFilename, L"Missing Shader File", MB_OK);
		return;
	}
	
	char* compileErrors = (char*)(errorMessage->GetBufferPointer());
	unsigned long long bufferSize = errorMessage->GetBufferSize();

	ofstream fout;
	fout.open("shader-error.txt");

	for (unsigned long long i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix) const
{
	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;	// Lock the constant buffer so it can be written to.
	HRESULT result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(matrixBuffer, 0);	// Unlock the constant buffer.
	unsigned int bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);	// Set the constant buffer in the vertex shader with the updated values.

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) const
{
	deviceContext->IASetInputLayout(layout);

	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}
