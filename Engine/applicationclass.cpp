#include "applicationclass.hpp"

ApplicationClass::ApplicationClass(int screenWidth, int screenHeight, HWND hwnd) {
	m_Direct3D = new D3DClass(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (not m_Direct3D->isInitialized) {
		MessageBox(hwnd, L"Couldn't Initialize Direct3D", L"Error", MB_OK);
		return;
	}

	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	m_Model = new ModelClass(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename);
	if (not m_Model->isInitialized)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return;
	}

	m_TextureShader = new TextureShaderClass(m_Direct3D->GetDevice(), hwnd);
	if (not m_TextureShader->isInitialized) {
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return;
	}

	m_ColorShader = new ColorShaderClass(m_Direct3D->GetDevice(), hwnd);
	if (not m_ColorShader->isInitialized) {
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return;
	}

	isInitialized = true;
}

ApplicationClass::~ApplicationClass() {
	if (m_ColorShader) {
		delete m_ColorShader;
		m_ColorShader = 0;
	}
	if (m_TextureShader){
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	if (m_Model) {
		delete m_Model;
		m_Model = 0;
	}
	if (m_Camera) {
		delete m_Camera;
		m_Camera = 0;
	}
	if (m_Direct3D) {
		delete m_Direct3D;
		m_Direct3D = 0;
		m_Camera = 0;
		m_Model = 0;
		m_ColorShader = 0;
	}
}

bool ApplicationClass::Render() {
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	m_Camera->Render();

	XMMATRIX worldMatrix = m_Direct3D->GetWorldMatrix();
	XMMATRIX viewMatrix = m_Camera->GetViewMatrix();
	XMMATRIX projectionMatrix = m_Direct3D->GetProjectionMatrix();

	m_Model->Render(m_Direct3D->GetDeviceContext());

	bool success = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (not success) { return false; }

	m_Direct3D->EndScene();
	return true;
}