#include "D3DObject.h"

using namespace DirectX;


D3DObject::D3DObject()
	: m_position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_IndexCount(),
	m_VertexStride(),
	m_bIsSetMaterial(false)
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

void D3DObject::GetMaterials(std::vector<Material>& vOut) const {
	for (auto &part : m_model.modelParts) {
		vOut.push_back(part.material);
	}
}

void D3DObject::SetMaterials(const std::vector<Material>& v)
{
	for (int i = 0; i < m_model.modelParts.size(); ++i) {
		m_model.modelParts[i].material = v[i];
	}
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
	m_bIsSetMaterial = true;
	m_material = material;
}

void D3DObject::SetTexture(ID3D11ShaderResourceView * texture)
{
	m_pTexture = texture;
}

void D3DObject::SetModel(Model && model)
{
	std::swap(m_model, model);
	model.modelParts.clear();
}

void D3DObject::SetModel(const Model & model)
{
	m_model = model;
}

DirectX::BoundingBox D3DObject::GetLocalBoundingBox() const
{
	BoundingBox box;
	m_model.boundingBox.Transform(box, XMLoadFloat4x4(&m_world));
	return box;
}

DirectX::BoundingBox D3DObject::GetBoundingBox() const
{
	return m_model.boundingBox;
}

DirectX::BoundingOrientedBox D3DObject::GetBoundingOrientedBox() const
{
	BoundingOrientedBox box;
	BoundingOrientedBox::CreateFromBoundingBox(box, m_model.boundingBox);
	box.Transform(box, XMLoadFloat4x4(&m_world));
	return box;
}

void D3DObject::Draw(ID3D11DeviceContext * deviceContext, BasicEffect& effect)
{
	UINT strides = m_model.vertexStride;
	UINT offsets = 0;

	for (auto& part : m_model.modelParts)
	{
		// Set vertex buffer and index buffer
		deviceContext->IASetVertexBuffers(0, 1, part.vertexBuffer.GetAddressOf(), &strides, &offsets);
		deviceContext->IASetIndexBuffer(part.indexBuffer.Get(), part.indexFormat, 0);

		// Update data and apply
		effect.SetWorldMatrix(XMLoadFloat4x4(&m_world));
		if (part.texDiffuse) {
			effect.SetTexture(part.texDiffuse.Get());
		}
		else {
			effect.SetTexture(m_pTexture.Get());
		}
		if (m_bIsSetMaterial) {
			effect.SetMaterial(m_material);
		}
		else {
			effect.SetMaterial(part.material);
		}

		effect.Apply(deviceContext);

		deviceContext->DrawIndexed(part.indexCount, 0, 0);
	}
}

