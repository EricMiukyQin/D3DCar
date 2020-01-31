#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>

class Camera
{
public:
	Camera();
	virtual ~Camera();

public:
	virtual void UpdateViewMatrix() = 0;

public:
	DirectX::XMFLOAT3 GetPosition() const;     // Get position
	DirectX::XMVECTOR GetPositionXM() const;

	DirectX::XMFLOAT3 GetRightVector() const;  // Get right vector
	DirectX::XMVECTOR GetRightVectorXM() const;

	DirectX::XMFLOAT3 GetUpVector() const;     // Get up vector
	DirectX::XMVECTOR GetUpVectorXM() const;

	DirectX::XMFLOAT3 GetLookVector() const;   // Get look vector
	DirectX::XMVECTOR GetLookVectorXM() const;

	float GetNearWindowWidth() const;          // Get near window width
	float GetNearWindowHeight() const;         // Get near window height
	float GetFarWindowWidth() const;           // Get far window width
	float GetFarWindowHeight() const;          // Get far window height

	DirectX::XMFLOAT4X4 GetViewMatrix() const; // Get view matrix
	DirectX::XMMATRIX GetViewMatrixXM() const;

	DirectX::XMFLOAT4X4 GetProjMatrix() const; // Get proj matrix
	DirectX::XMMATRIX GetProjMatrixXM() const;

	D3D11_VIEWPORT GetViewPort() const;        // Get view port

	// Set view port
	void SetViewPort(const D3D11_VIEWPORT& viewPort);
	void SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

	// Set frustum
	void SetFrustum(float fovY, float aspect, float nearZ, float farZ);

protected:
	DirectX::XMFLOAT3 m_position;              // Position
	DirectX::XMFLOAT3 m_right;                 // Right vector
	DirectX::XMFLOAT3 m_up;                    // Up vector
	DirectX::XMFLOAT3 m_look;                  // Look vector

	// Attribute of frustum
	float m_nearZ;
	float m_farZ;
	float m_aspect;
	float m_fovY;
	float m_nearWindowHeight;
	float m_farWindowHeight;

	DirectX::XMFLOAT4X4 m_view;                // View matrix
	DirectX::XMFLOAT4X4 m_proj;                // Proj matrix

	D3D11_VIEWPORT m_viewPort;                 // View port
};
