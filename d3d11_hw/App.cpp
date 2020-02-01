#include "App.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include "FirstPersonCamera.h"
#include "ThirdPersonCamera.h"

using namespace DirectX;


App::App(HINSTANCE hInstance)
	: D3DApp(hInstance),
	m_CameraMode(CameraMode::FirstPerson)
{
	m_pDaylight = std::make_unique<SkyRender>();

	m_pCar = std::make_unique<CarModel>();
	m_pGound = std::make_unique<D3DObject>();
	m_pHouse = std::make_unique<D3DObject>();

	m_normalMat.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_normalMat.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_normalMat.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	m_shadowMat.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_shadowMat.diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	m_shadowMat.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);
}

App::~App()
{
}

bool App::Init()
{
	if (!D3DApp::Init())
		return false;

	// Initialize all render states
	RenderStates::InitAll(m_pd3dDevice.Get());

	if (!m_BasicEffect.InitAll(m_pd3dDevice.Get()))
		return false;

	if (!m_SkyEffect.InitAll(m_pd3dDevice.Get()))
		return false;

	if (!InitResource())
		return false;

	// Initialize mouse
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);

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
		m_BasicEffect.SetProjMatrix(m_pCamera->GetProjMatrixXM());
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
		cameraPos.x -= 2.5f;
		cameraPos.y += 1.5f;
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
	m_BasicEffect.SetViewMatrix(m_pCamera->GetViewMatrixXM());
	m_BasicEffect.SetEyePos(m_pCamera->GetPositionXM());

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
			cam1st->LookTo(XMFLOAT3(m_pCar->GetPosition().x - 2.5f, 1.5f, m_pCar->GetPosition().z), m_pCar->GetDirection(), XMFLOAT3(0.0f, 1.0f, 0.0f));

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
			cam3rd->SetDistance(12.0f);
			cam3rd->SetDistanceMinMax(3.0f, 20.0f);

			m_CameraMode = CameraMode::ThirdPerson;
		}
	}

	// Close application
	if (keyState.IsKeyDown(Keyboard::Escape))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);
}

void App::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ********************
	// 1. Draw non-transparent objects
	//
	m_BasicEffect.SetRenderDefault(m_pd3dImmediateContext.Get());

	m_pCar->Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_pGound->Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_pHouse->Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);

	// ******************
	// 2. Draw shadows of opaque normal objects
	//

	m_BasicEffect.SetShadowState(true);
	m_BasicEffect.SetRenderNoDoubleBlend(m_pd3dImmediateContext.Get(), 0);

	m_pCar->SetMaterial(m_shadowMat);
	m_pHouse->SetMaterials(houseShadowMat);

	m_pCar->Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_pHouse->Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);

	m_BasicEffect.SetShadowState(false);
	m_pCar->SetMaterial(m_normalMat);
	m_pHouse->SetMaterials(houseMat);


	// ******************
	// 3. Draw sky box
	//
	m_SkyEffect.SetRenderDefault(m_pd3dImmediateContext.Get());
	m_pDaylight->Draw(m_pd3dImmediateContext.Get(), m_SkyEffect, *m_pCamera);

	m_pSwapChain->Present(0, 0);
}

bool App::InitResource()
{
	// ******************
	// Initialze skybox
	//

	HR(m_pDaylight->InitResource(m_pd3dDevice.Get(), m_pd3dImmediateContext.Get(),
		L"Texture\\daylight.jpg",
		5000.0f));

	// ******************
	// Initialize objects
	//

	// Car
	m_pCar->SetMaterial(m_normalMat);
	m_pCar->CreateCar(m_pd3dDevice.Get());

	// Ground
	ComPtr<ID3D11ShaderResourceView> texture;
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\Ground.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	m_pGound->SetModel(Model(m_pd3dDevice.Get(),
		Geometry::CreatePlane(XMFLOAT2(10000.0f, 10000.0f), XMFLOAT2(500.0f, 500.0f))));
	m_pGound->SetWorldMatrix(XMMatrixTranslation(0.0f, -2.0f, 0.0f));
	m_pGound->SetTexture(texture.Get());
	m_pGound->SetMaterial(m_normalMat);

	// House
	m_ObjReader.Read(L"Model\\house.mbo", L"Model\\house.obj");
	m_pHouse->SetModel(Model(m_pd3dDevice.Get(), m_ObjReader));
	m_pHouse->GetMaterials(houseMat);
	houseShadowMat = std::vector<Material> {houseMat.size(), m_shadowMat};

	XMMATRIX S = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	BoundingBox houseBox = m_pHouse->GetLocalBoundingBox();
	houseBox.Transform(houseBox, S);

	m_pHouse->SetWorldMatrix(S * XMMatrixTranslation(-70.0f, -(houseBox.Center.y - houseBox.Extents.y + 1.0f) - 1.0f, 70.0f));

	// ******************
	// Initialize camera
	//
	InitFirstPersonCamera();

	m_BasicEffect.SetViewMatrix(m_pCamera->GetViewMatrixXM());
	m_BasicEffect.SetEyePos(m_pCamera->GetPositionXM());

	m_pCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
	m_BasicEffect.SetProjMatrix(m_pCamera->GetProjMatrixXM());

	// ******************
	// Initialize non-change value
	//

	// Slightly higher to show shadows
	m_BasicEffect.SetShadowMatrix(XMMatrixShadow(XMVectorSet(0.0f, 0.5f, 0.0f, 0.99f), XMVectorSet(0.0f, 10.0f, -10.0f, 1.0f)));
	m_BasicEffect.SetRefShadowMatrix(XMMatrixShadow(XMVectorSet(0.0f, 0.5f, 0.0f, 0.99f), XMVectorSet(0.0f, 10.0f, 30.0f, 1.0f)));

	// dirLight
	DirectionalLight dirLight;
	dirLight.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	dirLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_BasicEffect.SetDirLight(0, dirLight);
	// pointLight
	PointLight pointLight;
	pointLight.position = XMFLOAT3(0.0f, 10.0f, -10.0f);
	pointLight.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	pointLight.diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	pointLight.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	pointLight.att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	pointLight.range = 2500.0f;
	m_BasicEffect.SetPointLight(0, pointLight);

	return true;
}

void App::InitFirstPersonCamera()
{
	m_CameraMode = CameraMode::FirstPerson;
	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
	m_pCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);

	// Set camera position
	XMFLOAT3 cameraPos = m_pCar->GetPosition();
	cameraPos.x -= 2.5f;
	cameraPos.y += 1.5f;
	camera->SetPosition(cameraPos);
	camera->LookTo(camera->GetPosition(), m_pCar->GetDirection(), XMFLOAT3(0.0f, 1.0f, 0.0f));
}

