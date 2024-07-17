#include "textureshaderclass.hpp"


TextureShaderClass::TextureShaderClass(ID3D11Device* device, HWND hwnd) {

	// Set the filename of the vertex shader.
	wchar_t vsFilename[128];
	int error = wcscpy_s(vsFilename, 128, L"../Engine/texture.vs");
	if (error != 0) { return; }

	// Set the filename of the pixel shader.
	wchar_t psFilename[128];
	error = wcscpy_s(psFilename, 128, L"../Engine/texture.ps");
	if (error != 0) { return; }

	// Initialize the vertex and pixel shaders.
	isInitialized = InitializeShader(device, hwnd, vsFilename, psFilename);
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture) {

	// Set the shader parameters that it will use for rendering.
	bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result) { return false; }

	RenderShader(deviceContext, indexCount);	// Now render the prepared buffers with the shader.
	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {
	bool success = SetVertexBuffer(device, hwnd, vsFilename);
	if (not success) { return false; }
	success = SetPixelBuffer(device, hwnd, psFilename);
	if (not success) { return false; }
	success = SetMatrixBuffer(device);
	if (not success) { return false; }
	success = SetSamplerDesc(device);
	return success;
}

D3D11_INPUT_ELEMENT_DESC TextureShaderClass::SetPolygon(LPCSTR name, UINT index, DXGI_FORMAT format, UINT inputSlot, UINT offset, D3D11_INPUT_CLASSIFICATION slotClass, UINT stepRate) {
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

HRESULT TextureShaderClass::VertexInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer) {
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	auto pLayout_pos = SetPolygon("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	auto pLayout_tex = SetPolygon("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2]{ pLayout_pos, pLayout_tex };
	
	unsigned int numElements = sizeof(polygonLayout) / sizeof(pLayout_pos);
	
	return device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
}

bool TextureShaderClass::SetVertexBuffer(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename) {
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	HRESULT result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); }
		else { MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK); }
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) { return false; }

	result = VertexInputLayout(device, vertexShaderBuffer);
	if (FAILED(result)) { return false; }

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	return true;
}

bool TextureShaderClass::SetPixelBuffer(ID3D11Device* device, HWND hwnd, WCHAR* psFilename) {
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	HRESULT result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, psFilename); }
		else { MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK); }
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) { return false; }

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	return true;
}

bool TextureShaderClass::SetMatrixBuffer(ID3D11Device* device) {
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc{};
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	HRESULT result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	return !FAILED(result);
}

bool TextureShaderClass::SetSamplerDesc(ID3D11Device* device) {
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
	HRESULT result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	return !FAILED(result);
}

void TextureShaderClass::ShutdownShader() {
	if (m_sampleState) {
		m_sampleState->Release();
		m_sampleState = 0;
	}
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {
	char* compileErrors = (char*)(errorMessage->GetBufferPointer());	// Get a pointer to the error message text buffer.
	unsigned long long bufferSize = errorMessage->GetBufferSize();	// Get the length of the message.
	
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

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture) const {
	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;	// Lock the constant buffer so it can be written to.
	HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;	// Get a pointer to the data in the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);	// Unlock the constant buffer.

	unsigned int bufferNumber = 0;	// Set the position of the constant buffer in the vertex shader.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);	// set the constant buffer in the vertex shader with the updated values.
	deviceContext->PSSetShaderResources(0, 1, &texture);	// Set shader texture resource in the pixel shader.

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) const {
	deviceContext->IASetInputLayout(m_layout);	// Set the vertex input layout.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);	// Set the sampler state in the pixel shader.
	deviceContext->DrawIndexed(indexCount, 0, 0);	// Render the triangle.
}
