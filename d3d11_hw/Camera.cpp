#include "Camera.h"

using namespace DirectX;


Camera::Camera()
	: m_position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_right(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_up(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_look(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_nearZ(), m_farZ(), m_fovY(), m_aspect(),
	m_nearWindowHeight(), m_farWindowHeight(), m_viewPort()
{
	XMStoreFloat4x4(&m_view, XMMatrixIdentity());
	XMStoreFloat4x4(&m_proj, XMMatrixIdentity());
}

Camera::~Camera()
{
}

DirectX::XMFLOAT3 Camera::GetPosition() const
{
	return m_position;
}

DirectX::XMFLOAT3 Camera::GetRightVector() const
{
	return m_right;
}

DirectX::XMFLOAT3 Camera::GetUpVector() const
{
	return m_up;
}

DirectX::XMFLOAT3 Camera::GetLookVector() const
{
	return m_look;
}

float Camera::GetNearWindowWidth() const
{
	return m_aspect * m_nearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return m_nearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return m_aspect * m_farWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return m_farWindowHeight;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix() const
{
	return m_view;
}

DirectX::XMFLOAT4X4 Camera::GetProjMatrix() const
{
	return m_proj;
}

D3D11_VIEWPORT Camera::GetViewPort() const
{
	return m_viewPort;
}

void Camera::SetViewPort(const D3D11_VIEWPORT & viewPort)
{
	m_viewPort = viewPort;
}

void Camera::SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	m_viewPort.TopLeftX = topLeftX;
	m_viewPort.TopLeftY = topLeftY;
	m_viewPort.Width = width;
	m_viewPort.Height = height;
	m_viewPort.MinDepth = minDepth;
	m_viewPort.MaxDepth = maxDepth;
}

void Camera::SetFrustum(float fovY, float aspect, float nearZ, float farZ)
{
	m_fovY = fovY;
	m_aspect = aspect;
	m_nearZ = nearZ;
	m_farZ = farZ;

	m_nearWindowHeight = 2.0f * m_nearZ * tanf(0.5f * m_fovY);
	m_farWindowHeight = 2.0f * m_farZ * tanf(0.5f * m_fovY);

	XMStoreFloat4x4(&m_proj, XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_nearZ, m_farZ));
}
