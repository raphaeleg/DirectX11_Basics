#include "lightshaderclass.hpp"

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor); // Set the shader parameters that it will use for rendering.
	if (result) { RenderShader(deviceContext, indexCount); }
	return result;
}

LightShaderClass::LightShaderClass(ID3D11Device* device, HWND hwnd){
	isInitialized = SetVertexBuffer(device, hwnd) 
		&& SetPixelBuffer(device, hwnd) 
		&& SetSamplerDesc(device) 
		&& SetMatrixBuffer(device) 
		&& SetLightBufferDesc(device);
}
bool LightShaderClass::SetVertexBuffer(ID3D11Device* device, HWND hwnd) {
	ID3D10Blob* errorMessage{};
	ID3D10Blob* vertexShaderBuffer = 0;
	HRESULT result = D3DCompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); }
		else { MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK); }
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

bool LightShaderClass::SetPixelBuffer(ID3D11Device* device, HWND hwnd) {
	ID3D10Blob* errorMessage{};
	ID3D10Blob* pixelShaderBuffer = 0;
	HRESULT result = D3DCompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, psFilename); }
		else { MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK); }
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result)) { return false; }

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	return true;
}

HRESULT LightShaderClass::VertexInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer) {
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	auto pLayout_position = SetPolygon("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	auto pLayout_tex = SetPolygon("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
	auto pLayout_normal = SetPolygon("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3] = { pLayout_position, pLayout_tex, pLayout_normal };
	unsigned int numElements = sizeof(polygonLayout) / sizeof(pLayout_position);

	return device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &layout);
}

D3D11_INPUT_ELEMENT_DESC LightShaderClass::SetPolygon(LPCSTR name, UINT index, DXGI_FORMAT format, UINT inputSlot, UINT offset, D3D11_INPUT_CLASSIFICATION slotClass, UINT stepRate) {
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

bool LightShaderClass::SetMatrixBuffer(ID3D11Device* device) {
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc{};
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	HRESULT result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	return !FAILED(result);
}

bool LightShaderClass::SetSamplerDesc(ID3D11Device* device) {
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	HRESULT result = device->CreateSamplerState(&samplerDesc, &sampleState);
	return !FAILED(result);
}

bool LightShaderClass::SetLightBufferDesc(ID3D11Device* device) {
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC lightBufferDesc{};
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	HRESULT result = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
	return !FAILED(result);
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {
	if (!errorMessage) {
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

LightShaderClass::~LightShaderClass() {
	if (lightBuffer){
		lightBuffer->Release();
		lightBuffer = 0;
	}
	if (matrixBuffer)	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (sampleState)	{
		sampleState->Release();
		sampleState = 0;
	}
	if (layout)	{
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

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
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
	deviceContext->PSSetShaderResources(0, 1, &texture);	// Set shader texture resource in the pixel shader.


	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }
	
	LightBufferType* dataPtr2 = (LightBufferType*)mappedResource.pData;	// Get a pointer to the data in the constant buffer.
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->padding = 0.0f;

	deviceContext->Unmap(lightBuffer, 0);
	bufferNumber = 0;	// Set the position of the light constant buffer in the pixel shader.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer);

	return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {
	deviceContext->IASetInputLayout(layout);	// Set the vertex input layout.
	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);
	deviceContext->PSSetSamplers(0, 1, &sampleState);	// Set the sampler state in the pixel shader.
	deviceContext->DrawIndexed(indexCount, 0, 0);	// Render the triangle.
}
