#include "D3DObject.h"

using namespace DirectX;


D3DObject::D3DObject()
	: m_position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_IndexCount(),
	m_VertexStride()
{
	XMStoreFloat4x4(&m_world, XMMatrixIdentity());
}

D3DObject::~D3DObject()
{
}

XMFLOAT3 D3DObject::GetPosition() const
{
	return m_position;
}

void D3DObject::SetWorldMatrix(const DirectX::XMFLOAT4X4 & world)
{
	m_world = world;
	m_position = XMFLOAT3(m_world(3, 0), m_world(3, 1), m_world(3, 2));
}

void XM_CALLCONV D3DObject::SetWorldMatrix(DirectX::FXMMATRIX world)
{
	XMStoreFloat4x4(&m_world, world);
}

void D3DObject::SetMaterial(const Material & material)
{
	m_material = material;
}

void D3DObject::SetTexture(ID3D11ShaderResourceView * texture)
{
	m_pTexture = texture;
}

void D3DObject::Draw(ID3D11DeviceContext * deviceContext, BasicEffect& effect)
{
	// Set vertex buffer and index buffer
	UINT strides = m_VertexStride;
	UINT offsets = 0;
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Update data and apply
	effect.SetWorldMatrix(XMLoadFloat4x4(&m_world));
	effect.SetTexture(m_pTexture.Get());
	effect.SetMaterial(m_material);
	effect.Apply(deviceContext);

	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

