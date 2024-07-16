#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class ColorShaderClass
{
public:
	ColorShaderClass(ID3D11Device*, HWND);
	ColorShaderClass(const ColorShaderClass&) { isInitialized = true; }
	~ColorShaderClass();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

	bool isInitialized = false;

private:
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX) const;
	void RenderShader(ID3D11DeviceContext*, int) const;

	bool SetVertexBuffer(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename);
	bool SetPixelBuffer(ID3D11Device* device, HWND hwnd, WCHAR* psFilename);
	HRESULT VertexInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer);
	D3D11_INPUT_ELEMENT_DESC SetPolygon(LPCSTR name, UINT index, DXGI_FORMAT format, UINT inputSlot, UINT offset, D3D11_INPUT_CLASSIFICATION slotClass, UINT stepRate);
	D3D11_BUFFER_DESC SetMatrixBuffer();

	ID3D11VertexShader* vertexShader = 0;
	ID3D11PixelShader* pixelShader = 0;
	ID3D11InputLayout* layout = 0;
	ID3D11Buffer* matrixBuffer = 0;
};
