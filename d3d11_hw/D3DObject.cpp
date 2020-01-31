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

void D3DObject::Draw(ID3D11DeviceContext* deviceContext)
{
	// Set Vertex buffer and Index buffer settings for input assembly stage
	UINT strides = m_VertexStride;  // Number of bytes crossed
	UINT offsets = 0;               // Starting offset
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Get previously bounded Constant buffer and modify it
	ComPtr<ID3D11Buffer> cBuffer = nullptr;
	deviceContext->VSGetConstantBuffers(0, 1, cBuffer.GetAddressOf());
	CBChangesEveryDrawing cbDrawing;
	XMMATRIX W = XMLoadFloat4x4(&m_world);
	cbDrawing.world = XMMatrixTranspose(W);
	cbDrawing.worldInvTranspose = XMMatrixInverse(nullptr, W);
	cbDrawing.material = m_material;

	// Update the Constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(deviceContext->Map(cBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesEveryDrawing), &cbDrawing, sizeof(CBChangesEveryDrawing));
	deviceContext->Unmap(cBuffer.Get(), 0);

	// Set texture
	deviceContext->PSSetShaderResources(0, 1, m_pTexture.GetAddressOf());

	// Draw
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}
