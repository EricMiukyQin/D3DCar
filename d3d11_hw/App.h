#pragma once

#include "d3dApp.h"
#include "CarModel.h"
#include "ObjReader.h"
#include "D3DObject.h"
#include "SkyRender.h"

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
	void InitFirstPersonCamera();

private:
	std::unique_ptr<CarModel> m_pCar;             // Car model
	std::unique_ptr<D3DObject> m_pGound;          // Grass object
	std::unique_ptr<D3DObject> m_pHouse;			  // House

	Material m_shadowMat;                         // Shadow material
	Material m_normalMat;					      // Normal material
	std::vector<Material> houseMat;
	std::vector<Material> houseShadowMat;

	BasicEffect m_BasicEffect;					  // Object rendering effects management

	std::shared_ptr<Camera> m_pCamera;			  // Camera
	CameraMode m_CameraMode;					  // Camera mode

	ObjReader m_ObjReader;						  // Model ObjReader

	SkyEffect m_SkyEffect;		                  // Sky dffect
	std::unique_ptr<SkyRender> m_pDaylight;		  // Sky box: day light
};

