#include "ThirdPersonCamera.h"

using namespace DirectX;

ThirdPersonCamera::ThirdPersonCamera()
	: Camera(), m_target_pos(), m_distance(), m_minDist(), m_maxDist(), m_theta(), m_phi()
{
}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

void ThirdPersonCamera::UpdateViewMatrix()
{
	// Update m_postion
	m_position = {
		m_target_pos.x + m_distance * sinf(m_phi) * cosf(m_theta),
		m_target_pos.y + m_distance * cosf(m_phi),
		m_target_pos.z + m_distance * sinf(m_phi) * sinf(m_theta)
	};

	// Calculate L, R, U
	XMVECTOR P = XMLoadFloat3(&m_position);
	XMVECTOR L = XMVector3Normalize(XMLoadFloat3(&m_target_pos) - P);
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), L));
	XMVECTOR U = XMVector3Cross(L, R);

	// Update m_right, m_up and m_look
	XMStoreFloat3(&m_right, R);
	XMStoreFloat3(&m_up, U);
	XMStoreFloat3(&m_look, L);

	// Update view matrix
	m_view = {
		m_right.x, m_up.x, m_look.x, 0.0f,
		m_right.y, m_up.y, m_look.y, 0.0f,
		m_right.z, m_up.z, m_look.z, 0.0f,
		-XMVectorGetX(XMVector3Dot(P, R)), -XMVectorGetX(XMVector3Dot(P, U)), -XMVectorGetX(XMVector3Dot(P, L)), 1.0f
	};
}

DirectX::XMFLOAT3 ThirdPersonCamera::GetTargetPosition() const
{
	return m_target_pos;
}

float ThirdPersonCamera::GetDistance() const
{
	return m_distance;
}

float ThirdPersonCamera::GetRotationX() const
{
	return m_phi;
}

float ThirdPersonCamera::GetRotationY() const
{
	return m_theta;
}

void ThirdPersonCamera::SetTargetPos(const XMFLOAT3 & target_pos)
{
	m_target_pos = target_pos;
}

void ThirdPersonCamera::SetDistance(float dist)
{
	m_distance = dist;
}

void ThirdPersonCamera::SetRotationX(float phi)
{
	m_phi = XMScalarModAngle(phi);
	Clamp(m_phi, XM_PI / 6, XM_PIDIV2);
}

void ThirdPersonCamera::SetRotationY(float theta)
{
	m_theta = XMScalarModAngle(theta);
}

void ThirdPersonCamera::SetDistanceMinMax(float minDist, float maxDist)
{
	m_minDist = minDist;
	m_maxDist = maxDist;
}

void ThirdPersonCamera::RotateX(float rad)
{
	m_phi -= rad;
	Clamp(m_phi, XM_PI / 6, XM_PIDIV2);
}

void ThirdPersonCamera::RotateY(float rad)
{
	m_theta = XMScalarModAngle(m_theta - rad);
}

void ThirdPersonCamera::Approach(float dist)
{
	m_distance += dist;
	Clamp(m_distance, m_minDist, m_maxDist);
}

void ThirdPersonCamera::Clamp(float & in, float left, float right)
{
	if (in < left) {
		in = left;
	}
	else if (in > right) {
		in = right;
	}
}
