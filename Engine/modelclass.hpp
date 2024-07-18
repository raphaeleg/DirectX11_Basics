#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include "textureclass.hpp"
using namespace DirectX;
using namespace std;

class ModelClass
{
public:
	ModelClass(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* textureFilename) { isInitialized = InitializeBuffers(device, deviceContext, modelFilename, textureFilename); }
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
		XMFLOAT3 normal{ 0.0f, 0.0f, -1.0f };

		VertexType() {};
		VertexType(XMFLOAT3 pos, XMFLOAT2 tex) : position(pos), texture(tex) {};
		VertexType(XMFLOAT3 pos, XMFLOAT2 tex, XMFLOAT3 norm) : position(pos), texture(tex), normal(norm) {};
	};
	struct ModelType {
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;

		XMFLOAT3 GetPosition() { return XMFLOAT3(x, y, z); }
		XMFLOAT2 GetTexture() { return XMFLOAT2(tu, tv); }
		XMFLOAT3 GetNormal() { return XMFLOAT3(nx, ny, nz); }
	};

	bool InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* textureFilename);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*) const;

	D3D11_BUFFER_DESC BufferDesc(UINT byteWidth) const;
	D3D11_SUBRESOURCE_DATA Data(const void* v) const;
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	bool LoadModel(char* filename);

	ID3D11Buffer* vertexBuffer{};
	ID3D11Buffer* indexBuffer{};
	TextureClass* m_Texture{};
	ModelType* m_model{};
	int vertexCount = 3;	// Set the number of vertices in the vertex array.
	int indexCount = 3;	// Set the number of indices in the index array.
};