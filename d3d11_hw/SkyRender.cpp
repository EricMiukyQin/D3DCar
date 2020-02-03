#include "SkyRender.h"
#include "Geometry.h"
#include "d3dUtil.h"
#include "DXTrace.h"
using namespace DirectX;
using namespace Microsoft::WRL;

HRESULT SkyRender::InitResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::wstring& cubemapFilename, float skySphereRadius, bool generateMips)
{
	// Reset
	m_pIndexBuffer.Reset();
	m_pVertexBuffer.Reset();
	m_pTextureCubeSRV.Reset();

	HRESULT hr;
	// Load texture
	if (cubemapFilename.substr(cubemapFilename.size() - 3) == L"dds")
	{
		hr = CreateDDSTextureFromFile(device,
			generateMips ? deviceContext : nullptr,
			cubemapFilename.c_str(),
			nullptr,
			m_pTextureCubeSRV.GetAddressOf());
	}
	else
	{
		hr = CreateWICTexture2DCubeFromFile(device,
			deviceContext,
			cubemapFilename,
			nullptr,
			m_pTextureCubeSRV.GetAddressOf(),
			generateMips);
	}

	if (hr != S_OK)
		return hr;

	return InitResource(device, skySphereRadius);
}

HRESULT SkyRender::InitResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::vector<std::wstring>& cubemapFilenames, float skySphereRadius, bool generateMips)
{
	// Reset
	m_pIndexBuffer.Reset();
	m_pVertexBuffer.Reset();
	m_pTextureCubeSRV.Reset();

	HRESULT hr;
	// Load texture
	hr = CreateWICTexture2DCubeFromFile(device,
		deviceContext,
		cubemapFilenames,
		nullptr,
		m_pTextureCubeSRV.GetAddressOf(),
		generateMips);
	if (hr != S_OK)
		return hr;

	return InitResource(device, skySphereRadius);
}

ID3D11ShaderResourceView* SkyRender::GetTextureCube()
{
	return m_pTextureCubeSRV.Get();
}

void SkyRender::Draw(ID3D11DeviceContext* deviceContext, SkyEffect& skyEffect, const Camera& camera)
{
	UINT strides[1] = { sizeof(XMFLOAT3) };
	UINT offsets[1] = { 0 };
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), strides, offsets);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	XMFLOAT3 pos = camera.GetPosition();
	skyEffect.SetWorldViewProjMatrix(XMMatrixTranslation(pos.x, pos.y, pos.z) * camera.GetViewProjXM());
	skyEffect.SetTextureCube(m_pTextureCubeSRV.Get());
	skyEffect.Apply(deviceContext);
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

HRESULT SkyRender::InitResource(ID3D11Device* device, float skySphereRadius)
{
	HRESULT hr;
	auto sphere = Geometry::CreateSphere<VertexPos>(skySphereRadius);

	// Create vertext buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * (UINT)sphere.vertexVec.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = sphere.vertexVec.data();

	hr = device->CreateBuffer(&vbd, &InitData, &m_pVertexBuffer);
	if (hr != S_OK)
		return hr;

	// Create index buffer
	m_IndexCount = (UINT)sphere.indexVec.size();

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * m_IndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags = 0;

	InitData.pSysMem = sphere.indexVec.data();

	return device->CreateBuffer(&ibd, &InitData, &m_pIndexBuffer);
}