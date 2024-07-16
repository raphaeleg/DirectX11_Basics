#pragma once

#pragma comment(lib, "d3d11.lib") // all the Direct3D functionality for setting up and drawing 3D graphics in DirectX 11
#pragma comment(lib, "dxgi.lib") //  tools to interface with the hardware on the computer to obtain information about the refresh rate of the monitor, the video card being used, and so forth
#pragma comment(lib, "d3dcompiler.lib") // compiling shaders

#include <d3d11.h>
#include <directxmath.h>
#include <string>
using namespace DirectX;

static constexpr float PI = 3.141592654f;
struct RefreshRate {
    unsigned int numerator = 0;
    unsigned int denominator = 0;

    RefreshRate() {};
    RefreshRate(unsigned int n, unsigned int d) : numerator(n), denominator(d) {};
};

class D3DClass
{
public:
    bool isInitialized = false;
    LPCWSTR errorMessage = L"error";

    D3DClass(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
    D3DClass(const D3DClass&);
    ~D3DClass();

    void BeginScene(float r, float g, float b, float a);
    void EndScene();

    ID3D11Device* GetDevice() { return device; }
    ID3D11DeviceContext* GetDeviceContext() { return deviceContext; }
    XMMATRIX GetProjectionMatrix() const { return projectionMatrix; }
    XMMATRIX GetWorldMatrix() const { return worldMatrix; }
    XMMATRIX GetOrthoMatrix() const { return orthoMatrix; }

    void GetVideoCardInfo(char* cardName, int& memory) const;

    void SetBackBufferRenderTarget() { deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView); }
    void ResetViewport() { deviceContext->RSSetViewports(1, &viewport); }

private:
    bool vsync_enabled = false;
    int videoCardMemory = 0;
    char videoCardDescription[128]{};
    IDXGISwapChain* swapChain{};
    ID3D11Device* device{};
    ID3D11DeviceContext* deviceContext{};
    ID3D11RenderTargetView* renderTargetView{};
    ID3D11Texture2D* depthStencilBuffer{};
    ID3D11DepthStencilState* depthStencilState{};
    ID3D11DepthStencilView* depthStencilView{};
    ID3D11RasterizerState* rasterState{};
    XMMATRIX projectionMatrix{};
    XMMATRIX worldMatrix = XMMatrixIdentity();
    XMMATRIX orthoMatrix{};
    D3D11_VIEWPORT viewport{};

    RefreshRate GetRefreshRate(unsigned int screenWidth, unsigned int screenHeight);
    bool CreateSwapChainDesc(int screenWidth, int screenHeight, HWND hwd, bool fullscreen);
    bool CreateRenderTargetView();
    bool CreateBuffers(int screenWidth, int screenHeight);
    bool CreateDepthBuffer(int screenWidth, int screenHeight);
    bool CreateDepthStencil();
    bool CreateDepthStencilView();
    bool CreateRaster();
    void SetViewport(float screenWidth, float screenHeight);
    void SetProjectionMatrix(float screenWidth, float screenHeight, float screenDepth, float screenNear);
};
