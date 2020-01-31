#pragma once

#include <vector>
#include "D3DObject.h"


class CarModel
{
protected:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	class Component : public D3DObject {
	public:
		Component();
		~Component();

	public:
		void UpdateLocalWorldMatrix();

	public:
		DirectX::XMFLOAT4X4 local_world;   // Local world matrix in car coordinate
		DirectX::XMFLOAT4X4 local_scale;   // Local scaling matrix in car coordinate
		DirectX::XMFLOAT4X4 local_rot;     // Local rotation matrix in car coordinate
		DirectX::XMFLOAT4X4 local_trans;   // Local translation matrix in car coordinate
	};

	enum class MoveState {
		Forward,
		Backward,
		Stop
	};

public:
	CarModel();
	~CarModel();

public:
	DirectX::XMFLOAT4X4 GetWorldMatrix() const;     // Get world matrix
	DirectX::XMFLOAT3 GetPosition() const;          // Get postion
	DirectX::XMFLOAT3 GetDirection() const;         // Get heading direction
	void UpdateWorldMatrix();                       // Update world matrix
	void CreateCar(ID3D11Device* device);           // Create Car Model
	void Move(float dt);                            // Move (forward or backward)
	void Turn(float& totalDegree, float dt);        // Turn (left or right, only when moving)
	void Draw(ID3D11DeviceContext * deviceContext, BasicEffect& effect);
	void SetMoveForward();                          // Set move state -- Forward
	void SetMoveBackward();                         // Set move state -- Backward
	void SetStop();                                 // Set move state -- Stop

private:
	void CreateCarBase(ID3D11Device* device);
	void CreateCarBody(ID3D11Device* device);
	void CreateFrontLeftWheel(ID3D11Device* device);
	void CreateFrontRightWheel(ID3D11Device* device);
	void CreateBackLeftWheel(ID3D11Device* device);
	void CreateBackRightWheel(ID3D11Device* device);
	void UpdateComponentsWorldMatrix();             // Update component's world matrix

private:
	std::vector<std::unique_ptr<Component>> m_car;         // Car model
	DirectX::XMFLOAT4X4 m_car_world;                       // Local world matrix in car coordinate
	DirectX::XMFLOAT4X4 m_car_scale;                       // Local scaling matrix in car coordinate
	DirectX::XMFLOAT4X4 m_car_rot;                         // Local rotation matrix in car coordinate
	DirectX::XMFLOAT4X4 m_car_trans;                       // Local translation matrix in car coordinate
	DirectX::XMFLOAT3 m_car_position;                      // Position of car
	DirectX::XMFLOAT3 m_car_headingDirection;              // Heading direction of car (normalized)
	MoveState m_car_state;                                 // Car move state
	
	// Constant variables
	const float moveSpeed = 3.0f;                          // Move speed of car
	const float turnSpeed = 0.25f;                         // Turn speed of car
	const int NUM_PARTS_CAR = 6;                           // 6 parts: CarBase, CarBody, Front_Left_Wheel, Front_Right_Wheel, Back_Left_Wheel, Back_Right_Wheel
};
