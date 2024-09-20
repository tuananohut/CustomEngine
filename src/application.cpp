#include "headers/application.h"


Application::Application()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_GroundModel = nullptr;
	m_WallModel = nullptr;
	m_BathModel = nullptr;
	m_WaterModel = nullptr;
	m_RefractionTexture = nullptr;
	m_ReflectionTexture = nullptr;
	m_Light = nullptr;
	m_LightShader = nullptr;
	m_RefractionShader = nullptr;
	m_WaterShader = nullptr;
}


Application::Application(const Application& other) {}


Application::~Application() {}


bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128];
	char textureFilename[128];
	char textureFilename1[128];
	char textureFilename2[128];

	bool result;

	m_Direct3D = new D3D;
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_Camera = new Camera;

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();

	m_Camera->SetPosition(10.0f, 6.0f, -10.0f);
	m_Camera->SetRotation(0.0f, -40.0f, 0.0f);

	strcpy_s(modelFilename, "../CustomEngine/src/assets/models/ground.txt");
	strcpy_s(textureFilename, "../CustomEngine/src/assets/shaders/ground01.tga");

	strcpy_s(textureFilename1, "../CustomEngine/src/assets/shaders/palestine.tga");
	strcpy_s(textureFilename2, "../CustomEngine/src/assets/shaders/dirt02.tga");

	m_GroundModel = new Model;
	result = m_GroundModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(modelFilename, "../CustomEngine/src/assets/models/wall.txt");
	strcpy_s(textureFilename, "../CustomEngine/src/assets/shaders/wall01.tga");
	

	m_WallModel = new Model;
	result = m_WallModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(modelFilename, "../CustomEngine/src/assets/models/bath.txt");
	strcpy_s(textureFilename, "../CustomEngine/src/assets/shaders/marble01.tga");

	m_BathModel = new Model;
	result = m_BathModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(modelFilename, "../CustomEngine/src/assets/models/water.txt");
	strcpy_s(textureFilename, "../CustomEngine/src/assets/shaders/water01.tga");

	m_WaterModel = new Model;
	result = m_WaterModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_Light = new Light;
	
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.f, 1.f, 1.f, 1.f);
	m_Light->SetDirection(0.f, -1.f, 0.5f);

	m_RefractionTexture = new RenderTexture;
	result = m_RefractionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, 1);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the refraction render texture object.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_ReflectionTexture = new RenderTexture;
	result = m_ReflectionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, 1);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the reflection render texture object.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_LightShader = new LightShader;
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_RefractionShader = new RefractionShader;
	result = m_RefractionShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the refraction shader object.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_WaterShader = new WaterShader;
	result = m_WaterShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the water shader object.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_waterHeight = 2.75f;
	m_waterTranslation = 0.f;

	return true;
}


void Application::Shutdown()
{
	if (m_WaterShader)
	{
		m_WaterShader->Shutdown();
		delete m_WaterShader;
		m_WaterShader = nullptr;
	}

	if (m_RefractionShader)
	{
		m_RefractionShader->Shutdown();
		delete m_RefractionShader;
		m_RefractionShader = nullptr;
	}

	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = nullptr;
	}

	if (m_ReflectionTexture)
	{
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = nullptr;
	}

	if (m_RefractionTexture)
	{
		m_RefractionTexture->Shutdown();
		delete m_RefractionTexture;
		m_RefractionTexture = nullptr;
	}

	if (m_Light)
	{
		delete m_Light;
		m_Light = nullptr;
	}

	if (m_WaterModel)
	{
		m_WaterModel->Shutdown();
		delete m_WaterModel;
		m_WaterModel = nullptr;
	}

	if (m_BathModel)
	{
		m_BathModel->Shutdown();
		delete m_BathModel;
		m_BathModel = nullptr;
	}

	if (m_WallModel)
	{
		m_WallModel->Shutdown();
		delete m_WallModel;
		m_WallModel = nullptr;
	}

	if (m_GroundModel)
	{
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = nullptr;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}
}


bool Application::Frame(Input* Input)
{
	bool result;

	if (Input->IsEscapePressed())
	{
		return false;
	}

	m_waterTranslation += 0.001f;
	if (m_waterTranslation > 1.f)
	{
		m_waterTranslation -= 1.f;
	}

	result = RenderRefractionToTexture();
	if (!result)
	{
		return false;
	}

	result = RenderReflectionToTexture();
	if (!result)
	{
		return false;
	}

	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Application::RenderRefractionToTexture()
{
	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	XMFLOAT4 clipPlane;
	bool result;

	clipPlane = XMFLOAT4(0.f, -1.f, 0.f, m_waterHeight + 0.1f);

	m_RefractionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
	m_RefractionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

	m_Camera->Render();

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix = XMMatrixTranslation(0.f, 2.f, 0.f);

	m_BathModel->Render(m_Direct3D->GetDeviceContext());
	result = m_RefractionShader->Render(m_Direct3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BathModel->GetTexture(0), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), clipPlane);
	if (!result)
	{
		return false;
	}

	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();

	return true;
}

bool Application::RenderReflectionToTexture()
{
	XMMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix;
	bool result;

	m_ReflectionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
	m_ReflectionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

	m_Camera->RenderReflection(m_waterHeight);

	m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix = XMMatrixTranslation(0.f, 6.f, 8.f);

	m_WallModel->Render(m_Direct3D->GetDeviceContext());
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, m_WallModel->GetTexture(0), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}
	
	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();

	return true;
}

bool Application::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix;
	bool result;

	m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);

	m_Camera->Render();

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix = XMMatrixTranslation(0.f, 1.f, 0.f);

	m_GroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_GroundModel->GetTexture(0), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	m_Direct3D->GetWorldMatrix(worldMatrix);

	worldMatrix = XMMatrixTranslation(0.f, 6.f, 8.f);

	m_WallModel->Render(m_Direct3D->GetDeviceContext());
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_WallModel->GetTexture(0), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	m_Direct3D->GetWorldMatrix(worldMatrix);

	worldMatrix = XMMatrixTranslation(0.f, 2.f, 0.f);

	m_BathModel->Render(m_Direct3D->GetDeviceContext());
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BathModel->GetTexture(0), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	m_Direct3D->GetWorldMatrix(worldMatrix);

	m_Camera->GetReflectionViewMatrix(reflectionMatrix);

	worldMatrix = XMMatrixTranslation(0.f, m_waterHeight, 0.f);

	m_WaterModel->Render(m_Direct3D->GetDeviceContext());
	result = m_WaterShader->Render(m_Direct3D->GetDeviceContext(), m_WaterModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix, m_ReflectionTexture->GetShaderResourceView(), m_RefractionTexture->GetShaderResourceView(), m_WaterModel->GetTexture(0), m_waterTranslation, 0.01f);
	if (!result)
	{
		return false;
	}

	m_Direct3D->EndScene();

	return true;
}