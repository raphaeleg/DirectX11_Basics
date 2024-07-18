#pragma once

#include <directxmath.h>
using namespace DirectX;

static constexpr auto RADIANS = 0.0174532925f;

class CameraClass
{
public:
	CameraClass(XMFLOAT3 pos) : position(pos) {};
	CameraClass(const CameraClass&) {};
	~CameraClass() {};

	void SetPosition(float x, float y, float z) { position = XMFLOAT3(x, y, z); }
	void SetRotation(float x, float y, float z) { rotation = XMFLOAT3(x, y, z); }

	XMFLOAT3 GetPosition() const { return position; }
	XMFLOAT3 GetRotation() const { return rotation; }
	XMMATRIX GetViewMatrix() const { return viewMatrix; }

	void Render();

private:
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMMATRIX viewMatrix{};
};