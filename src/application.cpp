#include "headers/application.h"


Application::Application()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_WindowModel = nullptr;
	m_RenderTexture = nullptr;
	m_TextureShader = nullptr;
	m_GlassShader = nullptr;

	m_DirectSound = nullptr;
	m_TestSound1 = nullptr;
}


Application::Application(const Application& other) {}


Application::~Application() {}


bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128];
	char textureFilename[128];
	char textureFilename1[128];
	char textureFilename2[128];
	char soundFilename[128];

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

	strcpy_s(modelFilename, "../CustomEngine/assets/models/cube.txt");
	
	strcpy_s(textureFilename, "../CustomEngine/assets/textures/stone01.tga");
	strcpy_s(textureFilename1, "../CustomEngine/assets/textures/icebump01.tga");
	strcpy_s(textureFilename2, "../CustomEngine/assets/textures/dirt02.tga");

	m_Model = new Model;
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(modelFilename, "../CustomEngine/assets/models/square.txt");
	strcpy_s(textureFilename, "../CustomEngine/assets/textures/ice01.tga");
	strcpy_s(textureFilename1, "../CustomEngine/assets/textures/icebump01.tga");
	
	m_WindowModel = new Model;
	result = m_WindowModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_RenderTexture = new RenderTexture;
	result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, 1);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render texture object.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_TextureShader = new TextureShader;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_GlassShader = new GlassShader;
	result = m_GlassShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the glass shader object.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}


	/*  */

	m_DirectSound = new DirectSound;
	result = m_DirectSound->Initialize(hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize direct sound.", L"Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// m_TestSound1 = new Sound3D;
	// 
	// strcpy_s(soundFilename, "../CustomEngine/assets/sounds/sound01.wav");
	// 
	// result = m_TestSound1->LoadTrack(m_DirectSound->GetDirectSound(), soundFilename, 0);
	// if (!result)
	// {
	// 	MessageBox(hwnd, L"Could not load the test 1 sound.", L"Error", MB_ICONERROR | MB_OK);
	// 	return false;
	// }
	// 
	// m_TestSound1->Update3DPosition(-1.f, 0.f, -1.f);
	// 
	// m_TestSound1->PlayTrack();





	// m_TestSound2 = new Sound3D;
	// 
	// strcpy_s(soundFilename, "../CustomEngine/assets/sounds/sound02.wav");
	// 
	// result = m_TestSound2->LoadTrack(m_DirectSound->GetDirectSound(), soundFilename, 0);
	// if (!result)
	// {
	// 	MessageBox(hwnd, L"Could not load the test 2 sound.", L"Error", MB_ICONERROR | MB_OK);
	// 	return false;
	// }
	// 
	// m_TestSound2->Update3DPosition(1.f, 0.f, -1.f);
	// 
	// m_TestSound2->PlayTrack();





	m_TestSound3 = new Sound3D;

	strcpy_s(soundFilename, "../CustomEngine/assets/sounds/sound03.wav");

	result = m_TestSound3->LoadTrack(m_DirectSound->GetDirectSound(), soundFilename, 0);
	if (!result)
	{
		MessageBox(hwnd, L"Could not load the test 3 sound.", L"Error", MB_ICONERROR | MB_OK);
		return false;
	}

	m_TestSound3->Update3DPosition(-1.f, 0.f, 1.f);

	m_TestSound3->PlayTrack();
	

	/*  */

	return true;
}


void Application::Shutdown()
{
	/**/

	if (m_TestSound1)
	{
		m_TestSound1->StopTrack();

		m_TestSound1->ReleaseTrack();
		delete m_TestSound1;
		m_TestSound1 = nullptr;
	}

	if (m_TestSound2)
	{
		m_TestSound2->StopTrack();

		m_TestSound2->ReleaseTrack();
		delete m_TestSound2;
		m_TestSound2 = nullptr;
	}

	if (m_TestSound1)
	{
		m_TestSound3->StopTrack();

		m_TestSound3->ReleaseTrack();
		delete m_TestSound3;
		m_TestSound3 = nullptr;
	}

	if (m_DirectSound)
	{
		m_DirectSound->Shutdown();
		delete m_DirectSound;
		m_DirectSound = nullptr;
	}

	/**/

	if (m_GlassShader)
	{
		m_GlassShader->Shutdown();
		delete m_GlassShader;
		m_GlassShader = nullptr;
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

	if (m_WindowModel)
	{
		m_WindowModel->Shutdown();
		delete m_WindowModel;
		m_WindowModel = nullptr;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
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
	static float rotation = 0.f;
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

	result = RenderSceneToTexture(rotation);
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

bool Application::RenderSceneToTexture(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	bool result;

	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix = XMMatrixRotationY(rotation);

	m_Model->Render(m_Direct3D->GetDeviceContext());
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(0));
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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	float refractionScale;
	bool result;

	refractionScale = 0.1f;

	m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix = XMMatrixRotationY(rotation);

	m_Model->Render(m_Direct3D->GetDeviceContext());
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(0));
	if (!result)
	{
		return false;
	}

	worldMatrix = XMMatrixTranslation(0.f, 0.f, -1.5f);

	m_WindowModel->Render(m_Direct3D->GetDeviceContext());
	result = m_GlassShader->Render(m_Direct3D->GetDeviceContext(), m_WindowModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_WindowModel->GetTexture(0), m_WindowModel->GetTexture(1), m_RenderTexture->GetShaderResourceView(), refractionScale);
	if (!result)
	{
		return false;
	}

	m_Direct3D->EndScene();

	return true;
}