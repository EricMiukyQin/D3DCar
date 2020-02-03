#pragma once

#include <vector>
#include <string>
#include "Effects.h"
#include "Camera.h"

class SkyRender
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	SkyRender() = default;
	~SkyRender() = default;

	// No copy, but move
	SkyRender(const SkyRender&) = delete;
	SkyRender& operator=(const SkyRender&) = delete;
	SkyRender(SkyRender&&) = default;
	SkyRender& operator=(SkyRender&&) = default;

	// Need to provide the complete skybox texture or the already created skybox texture .dds file
	HRESULT InitResource(ID3D11Device* device,
		ID3D11DeviceContext* deviceContext,
		const std::wstring& cubemapFilename,
		float skySphereRadius,		// Sky ball radius
		bool generateMips = false);	// Mipmaps are not generated for static skyboxes by default

	// Need to provide six square maps of the skybox
	HRESULT InitResource(ID3D11Device* device,
		ID3D11DeviceContext* deviceContext,
		const std::vector<std::wstring>& cubemapFilenames,
		float skySphereRadius,		// Sky ball radius
		bool generateMips = false);	// Mipmaps are not generated for static skyboxes by default

	ID3D11ShaderResourceView* GetTextureCube();

	void Draw(ID3D11DeviceContext* deviceContext, SkyEffect& skyEffect, const Camera& camera);
private:
	HRESULT InitResource(ID3D11Device* device, float skySphereRadius);

private:
	UINT m_IndexCount;
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;
	ComPtr<ID3D11ShaderResourceView> m_pTextureCubeSRV;
};

