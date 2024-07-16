#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <vector>
using namespace DirectX;

class ModelClass
{
public:
	ModelClass(ID3D11Device* device) { isInitialized = InitializeBuffers(device); }
	ModelClass(const ModelClass&) : isInitialized(true) {}
	~ModelClass() { ShutdownBuffers(); }
	void Render(ID3D11DeviceContext* deviceContext) { RenderBuffers(deviceContext); }

	int GetIndexCount() const { return indexCount; }
	bool isInitialized = false;

private:
	struct VertexType {
		XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
		XMFLOAT4 color{ 0.0f, 0.0f, 0.0f, 0.0f };

		VertexType() {};
		VertexType(XMFLOAT3 pos, XMFLOAT4 col) : position(pos), color(col) {};
	};

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*) const;

	D3D11_BUFFER_DESC BufferDesc(UINT byteWidth) const;
	D3D11_SUBRESOURCE_DATA Data(const void* v) const;

	ID3D11Buffer* vertexBuffer{};
	ID3D11Buffer* indexBuffer{};
	const int vertexCount = 3;	// Set the number of vertices in the vertex array.
	const int indexCount = 3;	// Set the number of indices in the index array.
};