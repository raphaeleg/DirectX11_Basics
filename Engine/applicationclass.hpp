#pragma once
#include <windows.h>
#include "d3dclass.hpp"
#include "cameraclass.hpp"
#include "modelclass.hpp"
#include "colorshaderclass.hpp"
#include "textureshaderclass.hpp"
#include "lightshaderclass.hpp"
#include "lightclass.hpp"

static constexpr bool FULL_SCREEN = false;
static constexpr bool VSYNC_ENABLED = true;
static constexpr float SCREEN_DEPTH = 1000.0f;
static constexpr float SCREEN_NEAR = 0.3f;

class ApplicationClass
{
public:
	bool isInitialized = false;

	ApplicationClass(int, int, HWND);
	ApplicationClass(const ApplicationClass&) { isInitialized = true; }
	~ApplicationClass();

	bool Frame();
private:
	D3DClass* m_Direct3D = 0;
	CameraClass* m_Camera = 0;
	ModelClass* m_Model = 0;
	ColorShaderClass* m_ColorShader = 0;
	TextureShaderClass* m_TextureShader = 0;
	char textureFilename[128] = "../Engine/data/stone01.tga";
	LightShaderClass* m_LightShader = 0;
	LightClass* m_Light = 0;

	bool Render(float);
	void Delete(const void *);
};