#pragma once
#include <windows.h>

static constexpr bool FULL_SCREEN = false;
static constexpr bool VSYNC_ENABLED = true;
static constexpr float SCREEN_DEPTH = 1000.0f;
static constexpr float SCREEN_NEAR = 0.3f;

class ApplicationClass
{
public:
	bool isInitialized = false;

	ApplicationClass(int, int, HWND) { isInitialized = true;  }
	ApplicationClass(const ApplicationClass&) { isInitialized = true; }
	~ApplicationClass() {}

	bool Frame() { return true; }
private:
	bool Render() { return true; }
};