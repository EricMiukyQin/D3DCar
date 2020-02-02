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
	bool InitGameObjects();
	void InitFirstPersonCamera();
	void InitEffects();
	void InitLight();

private:
	// Objects
	std::unique_ptr<CarModel> m_pCar;             // Car model
	std::unique_ptr<D3DObject> m_pRoad;           // Road
	std::unique_ptr<D3DObject> m_pGrass_l;        // Grass left
	std::unique_ptr<D3DObject> m_pGrass_r;        // Grass right
	std::unique_ptr<D3DObject> m_pHouse;	      // House
	std::unique_ptr<D3DObject> m_pTree;	          // Tree

	// Material
	Material m_shadowMat;                         // Shadow material
	Material m_normalMat;					      // Normal material
	std::vector<Material> m_houseMat;
	std::vector<Material> m_houseShadowMat;
	std::vector<Material> m_treeMat;
	std::vector<Material> m_treeShadowMat;

	// Camera
	std::shared_ptr<Camera> m_pCamera;			  // Camera
	CameraMode m_CameraMode;					  // Camera mode

	// Model ObjReader
	ObjReader m_ObjReader;						  // Model ObjReader

	// Effect
	BasicEffect m_BasicEffect;					  // Object rendering effects management
	SkyEffect m_SkyEffect;		                  // Sky dffect
	std::unique_ptr<SkyRender> m_pDaylight;		  // Sky box: day light
};

