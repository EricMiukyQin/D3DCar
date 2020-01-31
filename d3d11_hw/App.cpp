#include "App.h"
#include "d3dUtil.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "FirstPersonCamera.h"
#include "ThirdPersonCamera.h"

using namespace DirectX;


App::App(HINSTANCE hInstance)
	: D3DApp(hInstance),
	m_CameraMode(CameraMode::FirstPerson),
	m_CBFrame(),
	m_CBOnResize()
{
	m_pCar = std::make_unique<CarModel>();
	m_pRefObj = std::make_unique<D3DObject>();
	m_pGrass = std::make_unique<D3DObject>();
}

App::~App()
{
}

bool App::Init()
{
	if (!D3DApp::Init()) return false;
	if (!InitEffect()) return false;
	if (!InitResource()) return false;

	// Initialize mouse, keyboard is not needed
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(Mouse::MODE_RELATIVE);

	return true;
}

void App::OnResize()
{
	D3DApp::OnResize();

	// Camera changes
	if (m_pCamera != nullptr)
	{
		m_pCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
		m_pCamera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
		m_CBOnResize.proj = XMMatrixTranspose(XMLoadFloat4x4(&m_pCamera->GetProjMatrix()));

		D3D11_MAPPED_SUBRESOURCE mappedData;
		HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[2].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
		memcpy_s(mappedData.pData, sizeof(CBChangesOnResize), &m_CBOnResize, sizeof(CBChangesOnResize));
		m_pd3dImmediateContext->Unmap(m_pConstantBuffers[2].Get(), 0);
	}
}

void App::UpdateScene(float dt)
{
	static float phi = 0.0f, theta = 0.0f;

	// Update mouse events to get relative offset
	Mouse::State mouseState = m_pMouse->GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();

	Keyboard::State keyState = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keyState);

	 //Car move and turn
	if (keyState.IsKeyDown(Keyboard::W))
	{
		m_pCar->Move(dt);
		m_pCar->SetMoveForward();
	}
	if (keyState.IsKeyDown(Keyboard::S))
	{
		m_pCar->Move(-dt);
		m_pCar->SetMoveBackward();
	}
	if (keyState.IsKeyDown(Keyboard::A)) {
		m_pCar->Turn(theta, -dt);
	}
	if (keyState.IsKeyDown(Keyboard::D)) {
		m_pCar->Turn(theta, dt);
	}
	if (!keyState.IsKeyDown(Keyboard::W) && !keyState.IsKeyDown(Keyboard::S)) {
		m_pCar->SetStop();
	}

	// Camera
	auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);
	auto cam3rd = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);

	if (m_CameraMode == CameraMode::FirstPerson) {
		// Set camera position
		XMFLOAT3 cameraPos = m_pCar->GetPosition();
		cameraPos.y += 2.0f;
		cam1st->SetPosition(cameraPos);

		// Field of view rotation to prevent sudden rotation caused by too large difference
		cam1st->RotateZ(mouseState.y * dt * 1.25f);
		cam1st->RotateY(mouseState.x * dt * 1.25f);
	}
	else {
		cam3rd->SetTargetPos(m_pCar->GetPosition());

		// Rotate around target
		cam3rd->RotateX(mouseState.y * dt * 1.25f);
		cam3rd->RotateY(mouseState.x * dt * 1.25f);
		cam3rd->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);
	}

	// Update view matrix
	m_pCamera->UpdateViewMatrix();
	XMStoreFloat4(&m_CBFrame.eyePos, XMLoadFloat3(&m_pCamera->GetPosition()));
	m_CBFrame.view = XMMatrixTranspose(XMLoadFloat4x4(&m_pCamera->GetViewMatrix()));

	// Reset scroll wheel value
	m_pMouse->ResetScrollWheelValue();

	 //Switch camera mode
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Tab)) {
		if (m_CameraMode == CameraMode::ThirdPerson) {
			// Switch to FirstPerson mode
			if (!cam1st)
			{
				cam1st.reset(new FirstPersonCamera);
					cam1st->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
					m_pCamera = cam1st;
			}
			cam1st->LookTo(XMFLOAT3(m_pCar->GetPosition().x, 2.0f, m_pCar->GetPosition().z), m_pCar->GetDirection(), XMFLOAT3(0.0f, 1.0f, 0.0f));

			m_CameraMode = CameraMode::FirstPerson;
		}
		else {
			// Switch to ThirdPerson mode
			if (!cam3rd)
			{
				cam3rd.reset(new ThirdPersonCamera);
				cam3rd->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
				m_pCamera = cam3rd;
			}
			cam3rd->SetTargetPos(m_pCar->GetPosition());
			cam3rd->SetDistance(8.0f);
			cam3rd->SetDistanceMinMax(3.0f, 20.0f);

			m_CameraMode = CameraMode::ThirdPerson;
		}
	}

	// Close application
	if (keyState.IsKeyDown(Keyboard::Escape))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);

	// Update Constant buffer for stuff that changes every frame
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesEveryFrame), &m_CBFrame, sizeof(CBChangesEveryFrame));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[1].Get(), 0);
}

void App::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&black));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ********************
	// 1. Draw non-transparent objects
	//
	m_pd3dImmediateContext->RSSetState(nullptr);
	m_pd3dImmediateContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

	m_pCar->Draw(m_pd3dImmediateContext.Get());
	m_pRefObj->Draw(m_pd3dImmediateContext.Get());
	m_pGrass->Draw(m_pd3dImmediateContext.Get());

	//// ********************
	//// 2. Draw transparent objects
	////
	//m_pd3dImmediateContext->RSSetState(RenderStates::RSNoCull.Get());
	//m_pd3dImmediateContext->OMSetBlendState(RenderStates::BSTransparent.Get(), nullptr, 0xFFFFFFFF);

	m_pSwapChain->Present(0, 0);
}


bool App::InitEffect()
{
	ComPtr<ID3DBlob> blob;

	// Create Vertex Shader
	HR(CreateShaderFromFile(L"..\\x64\\Debug\\Basic_VS_3D.cso", L"Basic_VS_3D.hlsl", "VS_3D", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));

	// Create Input Layout 
	HR(m_pd3dDevice->CreateInputLayout(VertexPosNormalTex::inputLayout, ARRAYSIZE(VertexPosNormalTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

	// Create Pixel Shader
	HR(CreateShaderFromFile(L"..\\x64\\Debug\\Basic_PS_3D.cso", L"Basic_PS_3D.hlsl", "PS_3D", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

	return true;
}


bool App::InitResource()
{
	// Constant buffer description
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// Create constant buffer
	cbd.ByteWidth = sizeof(CBChangesEveryDrawing);
	m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[0].GetAddressOf());
	cbd.ByteWidth = sizeof(CBChangesEveryFrame);
	m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[1].GetAddressOf());
	cbd.ByteWidth = sizeof(CBChangesOnResize);
	m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[2].GetAddressOf());
	cbd.ByteWidth = sizeof(CBChangesRarely);
	m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[3].GetAddressOf());


	// Create objects in game world
	m_pCar->CreateCar(m_pd3dDevice.Get());

	Material material{};  // Material same as car's
	material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	// RefObj
	ComPtr<ID3D11ShaderResourceView> texture;
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\WoodCrate.dds", nullptr, texture.GetAddressOf()));
	m_pRefObj->SetBuffer(m_pd3dDevice.Get(), Geometry::CreateBox());
	m_pRefObj->SetWorldMatrix(XMMatrixTranslation(-30.0f, 0.0f, 0.0f));
	m_pRefObj->SetTexture(texture.Get());
	m_pRefObj->SetMaterial(material);

	// Grass
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\Grass.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	m_pGrass->SetBuffer(m_pd3dDevice.Get(),
		Geometry::CreatePlane(XMFLOAT2(1000.0f, 1000.0f), XMFLOAT2(100.0f, 100.0f)));
	m_pGrass->SetWorldMatrix(XMMatrixTranslation(0.0f, -2.0f, 0.0f));
	m_pGrass->SetTexture(texture.Get());
	m_pGrass->SetMaterial(material);

	///////// Initialize value in constant buffer

	// Initialize camera
	m_CameraMode = CameraMode::FirstPerson;
	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
	m_pCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->LookTo(XMFLOAT3(m_pCar->GetPosition().x, 2.0f, m_pCar->GetPosition().z), m_pCar->GetDirection(), XMFLOAT3(0.0f, 1.0f, 0.0f));

	// Initialize value that changes when window resize
	m_pCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
	m_CBOnResize.proj = XMMatrixTranspose(XMLoadFloat4x4(&m_pCamera->GetProjMatrix()));

	// Initialize that rarely changes
	// Ambient light (direction light)
	m_CBRarely.dirLight[0].ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_CBRarely.dirLight[0].diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_CBRarely.dirLight[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_CBRarely.dirLight[0].direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	// Point light
	m_CBRarely.pointLight[0].position = XMFLOAT3(0.0f, 10.0f, 0.0f);
	m_CBRarely.pointLight[0].ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_CBRarely.pointLight[0].diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_CBRarely.pointLight[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_CBRarely.pointLight[0].att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	m_CBRarely.pointLight[0].range = 25.0f;
	m_CBRarely.numDirLight = 1;
	m_CBRarely.numPointLight = 1;
	m_CBRarely.numSpotLight = 0;

	///////////// Update constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[2].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesOnResize), &m_CBOnResize, sizeof(CBChangesOnResize));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[2].Get(), 0);

	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[3].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesRarely), &m_CBRarely, sizeof(CBChangesRarely));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[3].Get(), 0);

	// Initialize all render states
	RenderStates::InitAll(m_pd3dDevice.Get());

	// Bind the required resources to each stage of the rendering pipeline
	// Set the element type and input layout
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());

	// Bind constant buffers and shaders
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffers[0].GetAddressOf());
	m_pd3dImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBuffers[1].GetAddressOf());
	m_pd3dImmediateContext->VSSetConstantBuffers(2, 1, m_pConstantBuffers[2].GetAddressOf());
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);

	m_pd3dImmediateContext->RSSetState(RenderStates::RSNoCull.Get());

	m_pd3dImmediateContext->PSSetConstantBuffers(0, 1, m_pConstantBuffers[0].GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffers[1].GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(3, 1, m_pConstantBuffers[3].GetAddressOf());
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

	m_pd3dImmediateContext->PSSetSamplers(0, 1, RenderStates::SSLinearWrap.GetAddressOf());
	m_pd3dImmediateContext->OMSetBlendState(RenderStates::BSTransparent.Get(), nullptr, 0xFFFFFFFF);

	return true;
}
