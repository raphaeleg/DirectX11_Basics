#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "inputclass.hpp"
#include "applicationclass.hpp"

class SystemClass {
public:
	bool isInitialized = false;

	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
private:
	void Init();
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	LPCWSTR m_applicationName = L"Engine";
	HINSTANCE m_hinstance = GetModuleHandle(NULL);
	HWND m_hwnd;

	InputClass* m_Input = 0;
	ApplicationClass* m_Application = 0;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;