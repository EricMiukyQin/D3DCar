#pragma once

#include "Effects.h"
#include "DXTrace.h"
#include "Geometry.h"


class D3DObject {
public:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	D3DObject();
	~D3DObject();

public:
	DirectX::XMFLOAT3 GetPosition() const;                     // Get postion
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);     // Set world matrix
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world); // Set world matrix
	void SetMaterial(const Material& material);                // Set Material
	void SetTexture(ID3D11ShaderResourceView* texture);        // Set Texture

public:
	template<class VertexType, class IndexType>
	void SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData);

	void Draw(ID3D11DeviceContext * deviceContext, BasicEffect& effect);

private:
	DirectX::XMFLOAT4X4 m_world;			       // World matrix
	DirectX::XMFLOAT3 m_position;                  // Position
	Material m_material;                           // Material
	ComPtr<ID3D11ShaderResourceView> m_pTexture;   // Texture
	ComPtr<ID3D11Buffer> m_pVertexBuffer;          // Vertex Buffer
	ComPtr<ID3D11Buffer> m_pIndexBuffer;	       // Index Buffer
	UINT m_VertexStride;					       // Index byte size
	UINT m_IndexCount;						       // Indexed array size of object
};

template<class VertexType, class IndexType>
void D3DObject::SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData) {
	// Release old resource
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// Check D3D device
	if (device == nullptr) return;

	// Vertex buffer description
	m_VertexStride = sizeof(VertexType);
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * m_VertexStride;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	// Create Vertex buffer
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	HR(device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

	// Index buffer description
	m_IndexCount = (UINT)meshData.indexVec.size();
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = m_IndexCount * sizeof(IndexType);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	// Create Index buffer
	InitData.pSysMem = meshData.indexVec.data();
	HR(device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
}

