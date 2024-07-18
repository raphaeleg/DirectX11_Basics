#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class LightShaderClass {
public:
    LightShaderClass(ID3D11Device* device, HWND hwnd);
    LightShaderClass(const LightShaderClass&) { isInitialized = true; };
    ~LightShaderClass();

    bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

    bool isInitialized = false;
private:
    struct MatrixBufferType {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };
    struct LightBufferType {
        XMFLOAT4 diffuseColor;
        XMFLOAT3 lightDirection;
        float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
    };
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);
    void RenderShader(ID3D11DeviceContext*, int);

    bool SetVertexBuffer(ID3D11Device* device, HWND hwnd);
    bool SetPixelBuffer(ID3D11Device* device, HWND hwnd);
    HRESULT VertexInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer);
    D3D11_INPUT_ELEMENT_DESC SetPolygon(LPCSTR name, UINT index, DXGI_FORMAT format, UINT inputSlot, UINT offset, D3D11_INPUT_CLASSIFICATION slotClass, UINT stepRate);
    bool SetMatrixBuffer(ID3D11Device* device);
    bool SetSamplerDesc(ID3D11Device* device);
    bool SetLightBufferDesc(ID3D11Device* device);

    wchar_t vsFilename[128] = L"../Engine/light.vs";
    wchar_t psFilename[128] = L"../Engine/light.ps";
    ID3D11VertexShader* vertexShader = 0;
    ID3D11PixelShader* pixelShader = 0;
    ID3D11InputLayout* layout = 0;
    ID3D11SamplerState* sampleState = 0;
    ID3D11Buffer* matrixBuffer = 0;
    ID3D11Buffer* lightBuffer = 0;
};