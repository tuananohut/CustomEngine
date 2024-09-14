#include "headers/application.h"


Application::Application()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_CubeModel = nullptr;
	m_FloorModel = nullptr;
	m_ConeModel = nullptr;
	m_SphereModel = nullptr;
	m_RenderTexture = nullptr;
	m_TextureShader = nullptr;
	m_ReflectionShader = nullptr;
	m_ColorShader = nullptr;
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

	strcpy_s(modelFilename, "../CustomEngine/src/assets/models/cube.txt");

	strcpy_s(textureFilename, "../CustomEngine/src/assets/shaders/stone01.tga");
	strcpy_s(textureFilename1, "../CustomEngine/src/assets/shaders/palestine.tga");
	strcpy_s(textureFilename2, "../CustomEngine/src/assets/shaders/dirt02.tga");

	m_CubeModel = new Model;
	result = m_CubeModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(modelFilename, "../CustomEngine/src/assets/models/floor.txt");
	
	strcpy_s(textureFilename, "../CustomEngine/src/assets/shaders/blue01.tga");

	m_FloorModel = new Model;
	result = m_FloorModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(modelFilename, "../CustomEngine/src/assets/models/cone.txt");

	m_ConeModel = new Model;
	result = m_ConeModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(modelFilename, "../CustomEngine/src/assets/models/sphere.txt");

	m_SphereModel = new Model;
	result = m_SphereModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}


	m_TextureShader = new TextureShader;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_RenderTexture = new RenderTexture;
	result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, 1);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render texture object.", L"Error", MB_OK);
		return false;
	}

	m_TextureShader = new TextureShader;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_ReflectionShader = new ReflectionShader;
	result = m_ReflectionShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the reflection shader object.", L"Error", MB_OK);
		return false;
	}

	m_ColorShader = new ColorShader;
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void Application::Shutdown()
{
	if (m_ReflectionShader)
	{
		m_ReflectionShader->Shutdown();
		delete m_ReflectionShader;
		m_ReflectionShader = nullptr;
	}

	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = nullptr;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = nullptr;
	}

	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = nullptr;
	}

	if (m_CubeModel)
	{
		m_CubeModel->Shutdown();
		delete m_CubeModel;
		m_CubeModel = nullptr;
	}

	if (m_FloorModel)
	{
		m_FloorModel->Shutdown();
		delete m_FloorModel;
		m_FloorModel = nullptr;
	}

	if (m_ConeModel)
	{
		m_ConeModel->Shutdown();
		delete m_ConeModel;
		m_ConeModel = nullptr;
	}

	if (m_SphereModel)
	{
		m_SphereModel->Shutdown();
		delete m_SphereModel;
		m_SphereModel = nullptr;
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
	static float rotation = 0.0f;
	bool result;


	if (Input->IsEscapePressed())
	{
		return false;
	}

	rotation -= 0.0174532925f * 0.25f;
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}

	result = RenderReflectionToTexture(rotation);
	if (!result)
	{
		return false;
	}

	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}


bool Application::RenderReflectionToTexture(float rotation)
{
	XMMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix;
	bool result;
	float blendAmount = 0.5f;

	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

	m_Camera->RenderReflection(-1.5f);

	m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix = XMMatrixRotationY(rotation) * worldMatrix;

	m_CubeModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	worldMatrix = XMMatrixRotationY(rotation) * XMMatrixTranslation(-5.f, 0.f, 0.f);
	m_ConeModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_ConeModel->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	worldMatrix = XMMatrixRotationY(rotation) * XMMatrixTranslation(5.f, 0.f, 0.f);
	m_SphereModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}
	
	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();

	return true;
}

bool Application::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, reflectionViewMatrix;
	bool result;

	m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix = XMMatrixRotationY(rotation);

	m_CubeModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	worldMatrix = XMMatrixRotationY(rotation) * XMMatrixTranslation(-5.f, 0.f, 0.f);
	m_ConeModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_ConeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	worldMatrix = XMMatrixRotationY(rotation) * XMMatrixTranslation(5.f, 0.f, 0.f);
	m_SphereModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	m_Direct3D->GetWorldMatrix(worldMatrix);
	worldMatrix = XMMatrixTranslation(0.f, -1.5f, 0.f);

	m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);

	m_FloorModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ReflectionShader->Render(m_Direct3D->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_FloorModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), reflectionViewMatrix);
	if (!result)
	{
		return false;
	}

	m_Direct3D->EndScene();

	return true;
}