#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include "textureclass.hpp"
using namespace DirectX;

class ModelClass
{
public:
	ModelClass(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename) { isInitialized = InitializeBuffers(device, deviceContext, textureFilename); }
	ModelClass(const ModelClass&) { isInitialized = true; }
	~ModelClass() { ShutdownBuffers(); }
	void Render(ID3D11DeviceContext* deviceContext) { RenderBuffers(deviceContext); }

	int GetIndexCount() const { return indexCount; }
	ID3D11ShaderResourceView* GetTexture() { return m_Texture->GetTexture(); }
	bool isInitialized = false;

private:
	struct VertexType {
		XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
		XMFLOAT2 texture{ 0.0f, 0.0f };

		VertexType() {};
		VertexType(XMFLOAT3 pos, XMFLOAT2 tex) : position(pos), texture(tex) {};
	};

	bool InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*) const;

	D3D11_BUFFER_DESC BufferDesc(UINT byteWidth) const;
	D3D11_SUBRESOURCE_DATA Data(const void* v) const;
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

	ID3D11Buffer* vertexBuffer{};
	ID3D11Buffer* indexBuffer{};
	TextureClass* m_Texture{};
	const int vertexCount = 3;	// Set the number of vertices in the vertex array.
	const int indexCount = 3;	// Set the number of indices in the index array.
};