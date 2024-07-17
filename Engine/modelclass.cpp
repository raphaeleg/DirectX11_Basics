#include "modelclass.hpp"

bool ModelClass::InitializeBuffers(ID3D11Device * device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
	bool success = LoadTexture(device, deviceContext, textureFilename);
	if (!success) { return false; }

	// Load the vertex array with data.
	VertexType bottomLeft{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) };
	VertexType topLeft{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) };
	VertexType topMiddle{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.5f, 0.0f) };
	VertexType bottomRight{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) };
	VertexType topRight{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)};
	VertexType* vertices = new VertexType[vertexCount]{ bottomLeft, topMiddle, bottomRight };
	if (not vertices) { return false; }

	unsigned long* indices = new unsigned long[indexCount]{ 0,1,2 };	// Create the index array.
	if (not indices) { return false; }

	D3D11_BUFFER_DESC vertexBufferDesc = BufferDesc(sizeof(VertexType) * vertexCount);
	D3D11_SUBRESOURCE_DATA vertexData = Data(vertices);
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result)) { return false; }

	D3D11_BUFFER_DESC indexBufferDesc = BufferDesc(sizeof(unsigned long) * indexCount);
	D3D11_SUBRESOURCE_DATA indexData = Data(indices);
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result)) { return false; }

	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

	return true;
}

D3D11_BUFFER_DESC ModelClass::BufferDesc(UINT byteWidth) const {
	// Set up the description of the static vertex/index buffer.
	D3D11_BUFFER_DESC v{};
	v.Usage = D3D11_USAGE_DEFAULT;
	v.ByteWidth = byteWidth;
	v.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	v.CPUAccessFlags = 0;
	v.MiscFlags = 0;
	v.StructureByteStride = 0;
	return v;
}

D3D11_SUBRESOURCE_DATA ModelClass::Data(const void* v) const {
	// Give the subresource structure a pointer to the vertex/index data.
	D3D11_SUBRESOURCE_DATA d{};
	d.pSysMem = v;
	d.SysMemPitch = 0;
	d.SysMemSlicePitch = 0;
	return d;
}

void ModelClass::ShutdownBuffers() {
	ReleaseTexture();
	if (indexBuffer) {
		indexBuffer->Release();
		indexBuffer = 0;
	}
	if (vertexBuffer) {
		vertexBuffer->Release();
		vertexBuffer = 0;
	}
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext) const {
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename) {
	m_Texture = new TextureClass(device, deviceContext, filename);
	return m_Texture->isInitialized;
}
void ModelClass::ReleaseTexture() {
	if (!m_Texture) { return; }
	delete m_Texture;
	m_Texture = 0;
}