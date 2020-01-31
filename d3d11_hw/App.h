#pragma once

#include "d3dApp.h"
#include "CarModel.h"
#include "D3DObject.h"

class Camera;


class App : public D3DApp {
protected:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	struct CBChangesEveryDrawing {
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldInvTranspose;
		Material material;
	};

	struct CBChangesEveryFrame {
		DirectX::XMMATRIX view;
		DirectX::XMFLOAT4 eyePos;
	};

	struct CBChangesOnResize {
		DirectX::XMMATRIX proj;
	};

	struct CBChangesRarely {
		DirectionalLight dirLight[10];
		PointLight pointLight[10];
		SpotLight spotLight[10];
		int numDirLight;
		int numPointLight;
		int numSpotLight;
		int pad;
	};

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
	bool InitEffect();
	bool InitResource();

private:
	ComPtr<ID3D11InputLayout> m_pVertexLayout;    // Vertex Layout
	ComPtr<ID3D11Buffer> m_pConstantBuffers[4];	  // Constant buffers
	ComPtr<ID3D11VertexShader> m_pVertexShader;	  // Vertex Shader
	ComPtr<ID3D11PixelShader> m_pPixelShader;	  // Pixel Shader

	ComPtr<ID3D11SamplerState> m_pSamplerState;	  // Sampler status

	CBChangesEveryFrame m_CBFrame;				  // Constant buffer of stuff changes every frame
	CBChangesOnResize m_CBOnResize;				  // Constant buffer of stuff changes on resize
	CBChangesRarely m_CBRarely;					  // Constant buffer of stuff that rarely changed

	std::unique_ptr<CarModel> m_pCar;             // Car model
	std::unique_ptr<D3DObject> m_pRefObj;         // Refer object
	std::unique_ptr<D3DObject> m_pGrass;          // Grass object

	std::shared_ptr<Camera> m_pCamera;			  // Camera
	CameraMode m_CameraMode;					  // Camera mode
};
