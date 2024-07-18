#pragma once
#include <directxmath.h>
using namespace DirectX;
class LightClass {
public:
    LightClass(XMFLOAT4 col, XMFLOAT3 dir) : diffuseColor(col), direction(dir) {};
    LightClass(const LightClass&) {};
    ~LightClass() {};

    void SetDiffuseColor(float x, float y, float z, float w) { diffuseColor = XMFLOAT4(x, y, z, w); }
    void SetDiffuseColor(float x, float y, float z) { direction = XMFLOAT3(x, y, z); }

    XMFLOAT4 GetDiffuseColor() const { return diffuseColor; }
    XMFLOAT3 GetDirection() const { return direction; }

private:
    XMFLOAT4 diffuseColor{ 0.0f,0.0f,0.0f,0.0f };
    XMFLOAT3 direction{ 0.0f,0.0f,0.0f };
};
