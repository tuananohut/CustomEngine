#include "headers/application.h"


Application::Application()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_Model1 = nullptr;
	m_TransparentShader = nullptr;
	m_TextureShader = nullptr;
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

	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->Render();

	strcpy_s(modelFilename, "../CustomEngine/src/assets/models/square.txt");

	strcpy_s(textureFilename, "../CustomEngine/src/assets/shaders/stone01.tga");
	strcpy_s(textureFilename1, "../CustomEngine/src/assets/shaders/palestine.tga");
	strcpy_s(textureFilename2, "../CustomEngine/src/assets/shaders/dirt.tga");

	m_Model = new Model;
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_Model1 = new Model;
	result = m_Model1->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
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

	m_TransparentShader = new TransparentShader;
	result = m_TransparentShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the transparent shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void Application::Shutdown()
{
	if (m_TransparentShader)
	{
		m_TransparentShader->Shutdown();
		delete m_TransparentShader;
		m_TransparentShader = nullptr;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = nullptr;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	if (m_Model1)
	{
		m_Model1->Shutdown();
		delete m_Model1;
		m_Model1 = nullptr;
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
	static float textureTranslation = 0.0f;
	bool result;


	if (Input->IsEscapePressed())
	{
		return false;
	}

	textureTranslation += 0.01f;
	if (textureTranslation > 1.0f)
	{
		textureTranslation -= 1.0f;
	}

	result = Render(textureTranslation);
	if (!result)
	{
		return false;
	}

	return true;
}


bool Application::Render(float textureTranslation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;
	float blendAmount = 0.5f;

	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Model->Render(m_Direct3D->GetDeviceContext());
	
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(0));
	if (!result)
	{
		return false;
	}
	
	worldMatrix = XMMatrixTranslation(1.0f, 0.0f, -1.0f);

	m_Direct3D->EnableAlphaBlending();

	m_Model1->Render(m_Direct3D->GetDeviceContext());
	result = m_TransparentShader->Render(m_Direct3D->GetDeviceContext(), m_Model1->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model1->GetTexture(2), blendAmount);
	if (!result)
	{
		return false;
	}

	m_Direct3D->DisableAlphaBlending();

	m_Direct3D->EndScene();

	return true;
}