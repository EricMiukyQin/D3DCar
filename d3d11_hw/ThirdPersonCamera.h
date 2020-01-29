#pragma once

#include "Camera.h"


class ThirdPersonCamera : public Camera
{
public:
	ThirdPersonCamera();
	~ThirdPersonCamera();

public:
	void UpdateViewMatrix() override;

public:
	DirectX::XMFLOAT3 GetTargetPosition() const;            // Get target position
	float GetDistance() const;                              // Get distance to target
	float GetRotationX() const;                             // Get rotation by x-axis
	float GetRotationY() const;                             // Get rotation by y-axis

	void SetTargetPos(const DirectX::XMFLOAT3& target_pos); // Set target position
	void SetDistance(float dist);                           // Set distance to target
	void SetRotationX(float phi);                           // Set rotation by x-axis ([pi/6, pi/2]))
	void SetRotationY(float theta);                         // Set rotation by y-axis
	void SetDistanceMinMax(float minDist, float maxDist);   // Set min and max limitations of distance
	
	void RotateX(float rad);                                // Rotate by x-axis ([pi/6, pi/2]))
	void RotateY(float rad);                                // Rotate by y-axis
	void Approach(float dist);                              // Approach to target ([m_minDist, m_maxDist])

private:
	void Clamp(float &in, float left, float right);         // Clamp input value into [left, right]

private:
	DirectX::XMFLOAT3 m_target_pos;          // Target position
	float m_distance;                        // Distance to target
	float m_phi;							 // Current rotate angle by x-axis based on world coordinate 
	float m_theta;                           // Current rotate angle by y-axis based on world coordinate 
	float m_minDist, m_maxDist;              // Min and max dist limitations
};

