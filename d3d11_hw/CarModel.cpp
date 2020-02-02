#include "CarModel.h"
#include "DXTrace.h"
#include "DDSTextureLoader.h"

using namespace DirectX;


CarModel::Component::Component()
{
	XMStoreFloat4x4(&local_world, XMMatrixIdentity());
	XMStoreFloat4x4(&local_scale, XMMatrixIdentity());
	XMStoreFloat4x4(&local_rot, XMMatrixIdentity());
	XMStoreFloat4x4(&local_trans, XMMatrixIdentity());
}

CarModel::Component::~Component()
{
}

void CarModel::Component::UpdateLocalWorldMatrix()
{
	XMStoreFloat4x4(
		&local_world,
		XMLoadFloat4x4(&local_scale) * XMLoadFloat4x4(&local_rot) * XMLoadFloat4x4(&local_trans)
	);
}

CarModel::CarModel()
	: m_car_position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_car_headingDirection(XMFLOAT3(-1.0f, 0.0f, 0.0f)),
	m_car_state(MoveState::Stop)
{
	XMStoreFloat4x4(&m_car_world, XMMatrixIdentity());
	XMStoreFloat4x4(&m_car_scale, XMMatrixIdentity());
	XMStoreFloat4x4(&m_car_rot, XMMatrixIdentity());
	XMStoreFloat4x4(&m_car_trans, XMMatrixIdentity());

	for (int i = 0; i < NUM_PARTS_CAR; ++i) {
		m_car.push_back(std::make_unique<Component>());
	}
}

CarModel::~CarModel()
{
}

XMFLOAT4X4 CarModel::GetWorldMatrix() const
{
	return m_car_world;
}

XMFLOAT3 CarModel::GetPosition() const
{
	return m_car_position;
}

XMFLOAT3 CarModel::GetDirection() const
{
	return m_car_headingDirection;
}

void CarModel::UpdateWorldMatrix()
{
	XMStoreFloat4x4(
		&m_car_world,
		XMLoadFloat4x4(&m_car_scale) * XMLoadFloat4x4(&m_car_rot) * XMLoadFloat4x4(&m_car_trans)
	);

	// When update car's world matrix, update
	// each compoent's world matrix at same time!!!
	UpdateComponentsWorldMatrix();
}

void CarModel::UpdateComponentsWorldMatrix()
{
	for (int i = 0; i < NUM_PARTS_CAR; ++i) {
		m_car[i]->SetWorldMatrix(XMLoadFloat4x4(&m_car[i]->local_world) * XMLoadFloat4x4(&m_car_world));
	}
}

void CarModel::CreateCar(ID3D11Device * device)
{
	// Create each component of car
	CreateCarBase(device);
	CreateCarBody(device);
	CreateFrontLeftWheel(device);
	CreateFrontRightWheel(device);
	CreateBackLeftWheel(device);
	CreateBackRightWheel(device);

	// Update each component's world matrix
	UpdateComponentsWorldMatrix();
}

void CarModel::Move(float dt)
{
	float distance = dt * moveSpeed;

	static float angle = 0.0f;
	angle += distance > 0 ? 0.02f : -0.02f;

	// Calculate wheels' rotation
	for (int i = 2; i < NUM_PARTS_CAR; ++i) {
		XMStoreFloat4x4(
			&m_car[i]->local_rot,
			XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationZ(angle));
		m_car[i]->UpdateLocalWorldMatrix();
	}

	// destination = distance * front + pos
	XMVECTOR destination = XMVectorMultiplyAdd(
		XMVectorReplicate(distance),
		XMLoadFloat3(&m_car_headingDirection),
		XMLoadFloat3(&m_car_position)
	);

	// Update m_car_position
	XMStoreFloat3(&m_car_position, destination);

	// Update m_car_trans
	XMStoreFloat4x4(&m_car_trans, XMMatrixTranslation(m_car_position.x, m_car_position.y, m_car_position.z));

	// Update m_car_world and each component's world matrix
	UpdateWorldMatrix();
}

void CarModel::Turn(float & worldDegree, float dt)
{
	float degree = dt * turnSpeed;
	if (m_car_state == MoveState::Forward || m_car_state == MoveState::Stop) {
		// Update m_car_headingDirection
		XMStoreFloat3(
			&m_car_headingDirection,
			XMVector3Normalize(XMVector3Transform(XMLoadFloat3(&m_car_headingDirection), XMMatrixRotationY(degree)))
		);

		// Update world degree outside
		worldDegree += degree;
	}
	else {
		// Update m_car_headingDirection
		XMStoreFloat3(
			&m_car_headingDirection,
			XMVector3Normalize(XMVector3Transform(XMLoadFloat3(&m_car_headingDirection), XMMatrixRotationY(-degree)))
		);

		// Update world degree outside
		worldDegree -= degree;
	}

	// Update m_car_rot
	XMStoreFloat4x4(&m_car_rot, XMMatrixRotationY(worldDegree));

	// Update m_car_world and each component's world matrix
	UpdateWorldMatrix();
}

void CarModel::Draw(ID3D11DeviceContext * deviceContext, BasicEffect& effect)
{
	for (int i = 0; i < NUM_PARTS_CAR; ++i) {
		m_car[i]->Draw(deviceContext, effect);
	}
}

void CarModel::SetMaterial(Material & material)
{
	for (int i = 0; i < NUM_PARTS_CAR; ++i) {
		m_car[i]->SetMaterial(material);
	}
}

void CarModel::SetMoveForward()
{
	m_car_state = MoveState::Forward;
}

void CarModel::SetMoveBackward()
{
	m_car_state = MoveState::Backward;
}

void CarModel::SetStop()
{
	m_car_state = MoveState::Stop;
}

void CarModel::CreateCarBase(ID3D11Device* device)
{
	// Set model
	m_car[0]->SetModel(Model(device, Geometry::CreateBox()));

	// Set local matrices
	XMStoreFloat4x4(&m_car[0]->local_scale, XMMatrixScaling(4.0f, 0.5f, 2.0f));
	m_car[0]->UpdateLocalWorldMatrix();

	// Set texture
	ComPtr<ID3D11ShaderResourceView> texture;
	HR(CreateDDSTextureFromFile(device, L"Texture\\car\\car_base.dds", nullptr, texture.GetAddressOf()));
	m_car[0]->SetTexture(texture.Get());
}

void CarModel::CreateCarBody(ID3D11Device * device)
{
	// Set model
	m_car[1]->SetModel(Model(device, Geometry::CreateBox()));

	// Set local matrices
	XMStoreFloat4x4(&m_car[1]->local_scale, XMMatrixScaling(3.0f, 0.5f, 2.0f));
	XMStoreFloat4x4(&m_car[1]->local_trans, XMMatrixTranslation(0.0f, 1.0f, 0.0f));
	m_car[1]->UpdateLocalWorldMatrix();

	// Set texture
	ComPtr<ID3D11ShaderResourceView> texture;
	HR(CreateDDSTextureFromFile(device, L"Texture\\car\\car_body.dds", nullptr, texture.GetAddressOf()));
	m_car[1]->SetTexture(texture.Get());
}

void CarModel::CreateFrontLeftWheel(ID3D11Device * device)
{
	// Set model
	m_car[2]->SetModel(Model(device, Geometry::CreateCylinder()));

	// Set local matrices
	XMStoreFloat4x4(&m_car[2]->local_scale, XMMatrixScaling(0.8f, 0.25f, 0.8f));
	XMStoreFloat4x4(&m_car[2]->local_rot, XMMatrixRotationX(XMConvertToRadians(90.0f)));
	XMStoreFloat4x4(&m_car[2]->local_trans, XMMatrixTranslation(-3.0f, -1.0f, -2.0f));
	m_car[2]->UpdateLocalWorldMatrix();

	// Set texture
	ComPtr<ID3D11ShaderResourceView> texture;
	HR(CreateDDSTextureFromFile(device, L"Texture\\car\\car_wheel.dds", nullptr, texture.GetAddressOf()));
	m_car[2]->SetTexture(texture.Get());
}

void CarModel::CreateFrontRightWheel(ID3D11Device * device)
{
	// Set model
	m_car[3]->SetModel(Model(device, Geometry::CreateCylinder()));

	// Set local matrices
	XMStoreFloat4x4(&m_car[3]->local_scale, XMMatrixScaling(0.8f, 0.25f, 0.8f));
	XMStoreFloat4x4(&m_car[3]->local_rot, XMMatrixRotationX(XMConvertToRadians(90.0f)));
	XMStoreFloat4x4(&m_car[3]->local_trans, XMMatrixTranslation(-3.0f, -1.0f, 2.0f));
	m_car[3]->UpdateLocalWorldMatrix();

	// Set texture
	ComPtr<ID3D11ShaderResourceView> texture;
	HR(CreateDDSTextureFromFile(device, L"Texture\\car\\car_wheel.dds", nullptr, texture.GetAddressOf()));
	m_car[3]->SetTexture(texture.Get());
}

void CarModel::CreateBackLeftWheel(ID3D11Device * device)
{
	// Set model
	m_car[4]->SetModel(Model(device, Geometry::CreateCylinder()));

	// Set local matrices
	XMStoreFloat4x4(&m_car[4]->local_scale, XMMatrixScaling(0.8f, 0.25f, 0.8f));
	XMStoreFloat4x4(&m_car[4]->local_rot, XMMatrixRotationX(XMConvertToRadians(90.0f)));
	XMStoreFloat4x4(&m_car[4]->local_trans, XMMatrixTranslation(3.0f, -1.0f, -2.0f));
	m_car[4]->UpdateLocalWorldMatrix();

	// Set texture
	ComPtr<ID3D11ShaderResourceView> texture;
	HR(CreateDDSTextureFromFile(device, L"Texture\\car\\car_wheel.dds", nullptr, texture.GetAddressOf()));
	m_car[4]->SetTexture(texture.Get());
}

void CarModel::CreateBackRightWheel(ID3D11Device * device)
{
	// Set model
	m_car[5]->SetModel(Model(device, Geometry::CreateCylinder()));

	// Set local matrices
	XMStoreFloat4x4(&m_car[5]->local_scale, XMMatrixScaling(0.8f, 0.25f, 0.8f));
	XMStoreFloat4x4(&m_car[5]->local_rot, XMMatrixRotationX(XMConvertToRadians(90.0f)));
	XMStoreFloat4x4(&m_car[5]->local_trans, XMMatrixTranslation(3.0f, -1.0f, 2.0f));
	m_car[5]->UpdateLocalWorldMatrix();

	// Set texture
	ComPtr<ID3D11ShaderResourceView> texture;
	HR(CreateDDSTextureFromFile(device, L"Texture\\car\\car_wheel.dds", nullptr, texture.GetAddressOf()));
	m_car[5]->SetTexture(texture.Get());
}