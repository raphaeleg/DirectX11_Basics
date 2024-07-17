#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class TextureShaderClass {
public:
	TextureShaderClass(ID3D11Device*, HWND);
	TextureShaderClass(const TextureShaderClass&) { isInitialized = true; }
	~TextureShaderClass() { ShutdownShader(); }
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

	bool isInitialized = false;

private:
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*) const;
	void RenderShader(ID3D11DeviceContext*, int) const;

	ID3D11VertexShader* m_vertexShader = 0;
	ID3D11PixelShader* m_pixelShader = 0;
	ID3D11InputLayout* m_layout = 0;
	ID3D11Buffer* m_matrixBuffer = 0;
	ID3D11SamplerState* m_sampleState = 0; // interface w/ texture shader

	bool SetVertexBuffer(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename);
	bool SetPixelBuffer(ID3D11Device* device, HWND hwnd, WCHAR* psFilename);
	D3D11_INPUT_ELEMENT_DESC SetPolygon(LPCSTR name, UINT index, DXGI_FORMAT format, UINT inputSlot, UINT offset, D3D11_INPUT_CLASSIFICATION slotClass, UINT stepRate);
	HRESULT VertexInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer);
	bool SetMatrixBuffer(ID3D11Device* device);
	bool SetSamplerDesc(ID3D11Device* device);
};