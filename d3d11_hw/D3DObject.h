#pragma once

#include "Model.h"


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
	void SetModel(Model&& model);                              // Set Model
	void SetModel(const Model& model);

	// Get and set materials for model's parts
	void GetMaterials(std::vector<Material>& vOut) const;      // Get materials of model parts
	void SetMaterials(const std::vector<Material>& v);         // Set materials of model parts

	// Get bounding box
	DirectX::BoundingBox GetLocalBoundingBox() const;
	DirectX::BoundingBox GetBoundingBox() const;
	DirectX::BoundingOrientedBox GetBoundingOrientedBox() const;

public:
	void Draw(ID3D11DeviceContext * deviceContext, BasicEffect& effect);

private:
	Model m_model;                                 // Model
	DirectX::XMFLOAT4X4 m_world;			       // World matrix
	DirectX::XMFLOAT3 m_position;                  // Position
	Material m_material;                           // Material
	ComPtr<ID3D11ShaderResourceView> m_pTexture;   // Texture
	ComPtr<ID3D11Buffer> m_pVertexBuffer;          // Vertex Buffer
	ComPtr<ID3D11Buffer> m_pIndexBuffer;	       // Index Buffer
	UINT m_VertexStride;					       // Index byte size
	UINT m_IndexCount;						       // Indexed array size of object

	bool m_bIsSetMaterial;                         // Check if to use m_material or model's own material
};

