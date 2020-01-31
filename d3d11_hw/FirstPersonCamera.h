#pragma once

#include "Camera.h"

class FirstPersonCamera : public Camera
{
public:
	FirstPersonCamera();
	~FirstPersonCamera();

public:
	void UpdateViewMatrix() override;

public:
	void SetPosition(const DirectX::XMFLOAT3& pos);    // Set position
	void LookTo(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT3& up);  // Looking direction
	void XM_CALLCONV LookTo(const DirectX::FXMVECTOR& pos, const DirectX::FXMVECTOR& to, const DirectX::FXMVECTOR& up);
	void RotateZ(float rad);                   // Rotate by z-axis
	void RotateY(float rad);                   // Rotate by y-axis
};
