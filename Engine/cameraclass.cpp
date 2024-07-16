#include "cameraclass.hpp"

void CameraClass::Render()
{
	XMFLOAT3 up{ 0.0f, 1.0f, 0.0f };	// Setup the vector that points upwards.
	XMVECTOR upVector = XMLoadFloat3(&up);

	XMVECTOR positionVector = XMLoadFloat3(&position);

	XMFLOAT3 lookAt{ 0.0f, 0.0f, 1.0f };	// Setup where the camera is looking by default.
	XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);

	float pitch = rotation.x * RADIANS;
	float yaw = rotation.y * RADIANS;
	float roll = rotation.z * RADIANS;
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);	// Finally create the view matrix from the three updated vectors.
}