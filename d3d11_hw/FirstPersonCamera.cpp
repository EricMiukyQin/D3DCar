#include "FirstPersonCamera.h"

using namespace DirectX;


FirstPersonCamera::FirstPersonCamera()
	: Camera()
{
}

FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::UpdateViewMatrix()
{
	// Calculate L, U, R (all normalized)
	XMVECTOR L = XMVector3Normalize(XMLoadFloat3(&m_look));
	XMVECTOR U = XMVector3Normalize(XMVector3Cross(L, XMLoadFloat3(&m_right)));
	XMVECTOR R = XMVector3Cross(U, L);

	// Position
	XMVECTOR P = XMLoadFloat3(&m_position);

	// Update m_right, m_up and m_look
	XMStoreFloat3(&m_right, R);
	XMStoreFloat3(&m_up, U);
	XMStoreFloat3(&m_look, L);

	// Update view matrix
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	m_view = {
		m_right.x, m_up.x, m_look.x, 0.0f,
		m_right.y, m_up.y, m_look.y, 0.0f,
		m_right.z, m_up.z, m_look.z, 0.0f,
		x, y, z, 1.0f
	};
}

void FirstPersonCamera::SetPosition(const XMFLOAT3 & pos)
{
	m_position = pos;
}

void FirstPersonCamera::LookTo(const XMFLOAT3 & pos, const XMFLOAT3 & to, const XMFLOAT3 & up)
{
	LookTo(XMLoadFloat3(&pos), XMLoadFloat3(&to), XMLoadFloat3(&up));
}

void XM_CALLCONV FirstPersonCamera::LookTo(const DirectX::FXMVECTOR & pos, const DirectX::FXMVECTOR & to, const DirectX::FXMVECTOR & up)
{
	// Calculate L, U, R (all normalized)
	XMVECTOR L = XMVector3Normalize(to);
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(up, L));
	XMVECTOR U = XMVector3Cross(L, R);

	// Update m_position, m_look, m_right and m_up
	XMStoreFloat3(&m_position, pos);
	XMStoreFloat3(&m_look, L);
	XMStoreFloat3(&m_right, R);
	XMStoreFloat3(&m_up, U);
}

void FirstPersonCamera::RotateZ(float rad)
{
	// Calculate L, U, R
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), rad);
	XMVECTOR U = XMVector3TransformNormal(XMLoadFloat3(&m_up), R);
	XMVECTOR L = XMVector3TransformNormal(XMLoadFloat3(&m_look), R);
	float cosPhi = XMVectorGetY(L);
	// Limit the upper and lower viewing angle Phi to [2pi / 9, 7pi / 9],
	// i.e. between cosine values [-cos (2pi / 9), cos (2pi / 9)]
	if (fabs(cosPhi) > cosf(XM_2PI / 9))
		return;

	// Update m_up and m_look
	XMStoreFloat3(&m_up, U);
	XMStoreFloat3(&m_look, L);
}

void FirstPersonCamera::RotateY(float rad)
{
	XMMATRIX R = XMMatrixRotationY(rad);

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), R));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));
}
