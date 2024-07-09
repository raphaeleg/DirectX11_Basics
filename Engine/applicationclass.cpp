#include "applicationclass.hpp"

ApplicationClass::ApplicationClass(int screenWidth, int screenHeight, HWND hwnd) {
	m_Direct3D = new D3DClass(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (not m_Direct3D->isInitialized) {
		MessageBox(hwnd, L"Couldn't Initialize Direct3D", L"Error", MB_OK);
		return;
	}
	isInitialized = true;
}

ApplicationClass::~ApplicationClass() {
	if (m_Direct3D) {
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}

bool ApplicationClass::Render() {
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
	m_Direct3D->EndScene();
	return true;
}