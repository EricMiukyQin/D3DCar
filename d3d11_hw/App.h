#pragma once

#include "d3dApp.h"
#include "CarModel.h"
#include "D3DObject.h"

class Camera;


class App : public D3DApp {
protected:
	// Camera mode
	enum class CameraMode {
		FirstPerson,
		ThirdPerson
	};

public:
	App(HINSTANCE hInstance);
	~App();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

private:
	bool InitResource();

private:
	std::unique_ptr<CarModel> m_pCar;             // Car model
	std::unique_ptr<D3DObject> m_pRefObj;         // Refer object
	std::unique_ptr<D3DObject> m_pGrass;          // Grass object

	BasicEffect m_BasicEffect;					  // Object rendering effects management

	std::shared_ptr<Camera> m_pCamera;			  // Camera
	CameraMode m_CameraMode;					  // Camera mode
};

