#include "d3dclass.hpp" 

D3DClass::D3DClass(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear) {	
	vsync_enabled = vsync;
	bool success = CreateSwapChainDesc(screenWidth, screenHeight, hwnd, fullscreen);
	if (!success) { return; }

	success = CreateRenderTargetView();
	if (!success) { return; }

	success = CreateBuffers(screenWidth, screenHeight);
	if (!success) { return; }
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);	// Bind the render target view and depth stencil buffer to the output render pipeline.

	success = CreateRaster();
	if (!success) { return; }
	deviceContext->RSSetState(rasterState);

	SetViewport((float)screenWidth, (float)screenHeight);
	deviceContext->RSSetViewports(1, &viewport);

	SetProjectionMatrix((float)screenWidth, (float)screenHeight, screenDepth, screenNear);

	isInitialized = true;
}

bool D3DClass::CreateSwapChainDesc(int screenWidth, int screenHeight, HWND hwnd, bool fullscreen) {
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;	// Set to a single back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// Set regular 32-bit surface for the back buffer.
	
	if (vsync_enabled) {
		RefreshRate device = GetRefreshRate(screenWidth, screenHeight);
		if (device.numerator == 0 && device.denominator == 0) { return false; }
		swapChainDesc.BufferDesc.RefreshRate.Numerator = device.numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = device.denominator;
	}
	else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;		// Set the handle for the window to render to.
	swapChainDesc.Windowed = !fullscreen;
	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	// Discard the back buffer contents after presenting.
	swapChainDesc.Flags = 0;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	auto result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);
	return !FAILED(result);
}

RefreshRate D3DClass::GetRefreshRate(unsigned int screenWidth, unsigned int screenHeight) {
	RefreshRate failed = RefreshRate();
	IDXGIFactory* factory{};	// graphics interface factory.
	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)) { return failed; }

	IDXGIAdapter* adapter{};	// Use factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) { return failed; }

	IDXGIOutput* adapterOutput{};	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) { return failed; }

	unsigned int numModes = 0;	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) { return failed; }

	 DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];	// Create a list to hold all the possible display modes for this monitor/video card combination.
	if (!displayModeList) { return failed; }

	// Fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) { return failed; }

	unsigned int numerator = 0;
	unsigned int denominator = 0;
	for (unsigned int i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == screenWidth) {
			if (displayModeList[i].Height == screenHeight) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
				break;
			}
		}
	}

	DXGI_ADAPTER_DESC adapterDesc{};
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) { return failed; }

	videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);	// Store the dedicated video card memory in megabytes.

	unsigned long long stringLength;	// Convert the name of the video card to a character array and store it.
	int error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) { return failed; }

	delete[] displayModeList;
	displayModeList = 0;
	adapterOutput->Release();
	adapterOutput = 0;
	adapter->Release();
	adapter = 0;
	factory->Release();
	factory = 0;

	return RefreshRate(numerator,denominator);
}

bool D3DClass::CreateRenderTargetView(){
	ID3D11Texture2D* backBufferPtr{};	// Get the pointer to the back buffer.
	HRESULT result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result)) { return false; }

	result = device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
	if (FAILED(result)) { return false; }

	backBufferPtr->Release();
	backBufferPtr = 0;
	return true;
}

bool D3DClass::CreateBuffers(int screenWidth, int screenHeight) {
	auto success = CreateDepthBuffer(screenWidth, screenHeight);
	if (!success) { return false; }
	success = CreateDepthStencil();
	if (!success) { return false; }
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	success = CreateDepthStencilView();
	return success;
}

bool D3DClass::CreateDepthBuffer(int screenWidth, int screenHeight) {
	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	auto result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	return !FAILED(result);
}

bool D3DClass::CreateDepthStencil() {
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};	// stencil buffer: Achieve effects such as motion blur, volumetric shadows, etc...
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	auto result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	return !FAILED(result);
}

bool D3DClass::CreateDepthStencilView() {
	// Set up desc so that Direct3D knows to use the depth buffer as a depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	auto result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	return !FAILED(result);
}

bool D3DClass::CreateRaster() {
	// Setup the raster description which will determine how and what polygons will be drawn. control over how polygons are rendered. 
	// We can do things like make our scenes render in wireframe mode or have DirectX draw both the front and back faces of polygons
	D3D11_RASTERIZER_DESC rasterDesc{};
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	auto result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	return !FAILED(result);
}

void D3DClass::SetViewport(float screenWidth, float screenHeight) {
	// Setup the viewport for rendering. can map clip space coordinates to the render target space
	viewport.Width = screenWidth;
	viewport.Height = screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
}

void D3DClass::SetProjectionMatrix(float screenWidth, float screenHeight, float screenDepth, float screenNear) {
	float fieldOfView = PI / 4.0f;
	float screenAspect = screenWidth / screenHeight;

	projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
	orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);
}

D3DClass::D3DClass(const D3DClass& other)
{
	isInitialized = true;
}


D3DClass::~D3DClass()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (swapChain)
	{
		swapChain->SetFullscreenState(false, NULL);
	}

	if (rasterState)
	{
		rasterState->Release();
		rasterState = 0;
	}

	if (depthStencilView)
	{
		depthStencilView->Release();
		depthStencilView = 0;
	}

	if (depthStencilState)
	{
		depthStencilState->Release();
		depthStencilState = 0;
	}

	if (depthStencilBuffer)
	{
		depthStencilBuffer->Release();
		depthStencilBuffer = 0;
	}

	if (renderTargetView)
	{
		renderTargetView->Release();
		renderTargetView = 0;
	}

	if (deviceContext)
	{
		deviceContext->Release();
		deviceContext = 0;
	}

	if (device)
	{
		device->Release();
		device = 0;
	}

	if (swapChain)
	{
		swapChain->Release();
		swapChain = 0;
	}
}

void D3DClass::BeginScene(float r, float g, float b, float a)
{
	float color[4] = { r,g,b,a };
	deviceContext->ClearRenderTargetView(renderTargetView, color);	// Clear the back buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);	// Clear the depth buffer.
}


void D3DClass::EndScene() {
	// Present the back buffer to the screen since rendering is complete.
	vsync_enabled ? swapChain->Present(1, 0) : swapChain->Present(0, 0);	// Lock to screen refresh rate or Present as fast as possible.
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory) const
{
	strcpy_s(cardName, 128, videoCardDescription);
	memory = videoCardMemory;
}