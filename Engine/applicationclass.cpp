#include "applicationclass.hpp"

ApplicationClass::ApplicationClass(int screenWidth, int screenHeight, HWND hwnd) {
	m_Direct3D = new D3DClass(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (not m_Direct3D->isInitialized) {
		MessageBox(hwnd, L"Couldn't Initialize Direct3D", L"Error", MB_OK);
		return;
	}

	XMFLOAT3 camPos{ 0.0f, 0.0f, -5.0f };
	m_Camera = new CameraClass(camPos);

	m_Model = new ModelClass(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename);
	if (not m_Model->isInitialized) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return;
	}

	m_LightShader = new LightShaderClass(m_Direct3D->GetDevice(), hwnd);
	if (not m_LightShader->isInitialized) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return;
	}

	XMFLOAT4 diffuseCol{ 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT3 lDirection{ 0.0f, 0.0f, 1.0f };
	m_Light = new LightClass(diffuseCol, lDirection);

	isInitialized = true;
}

ApplicationClass::~ApplicationClass() {
	Delete(m_Light);
	Delete(m_LightShader);
	Delete(m_Model);
	Delete(m_Camera);
	Delete(m_Direct3D);
}

void ApplicationClass::Delete(const void* item) {
	if (!item) { return; }
	delete item;
	item = 0;
}
bool ApplicationClass::Frame() {
	static float rotation = 0.0f;
	rotation -= 0.0174532925f * 0.5f;
	if (rotation < 0.0f) { rotation += 360.0f; }
	return Render(rotation); 
}

bool ApplicationClass::Render(float rotation ) {
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	m_Camera->Render();

	XMMATRIX worldMatrix = m_Direct3D->GetWorldMatrix();
	XMMATRIX viewMatrix = m_Camera->GetViewMatrix();
	XMMATRIX projectionMatrix = m_Direct3D->GetProjectionMatrix();

	worldMatrix = XMMatrixRotationY(rotation);

	m_Model->Render(m_Direct3D->GetDeviceContext());

	bool success = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
		m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (not success) { return false; }

	m_Direct3D->EndScene();
	return true;
}