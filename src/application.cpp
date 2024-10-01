#include "headers/application.h"

Application::Application()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_DepthShader = nullptr;

	m_XAudio = nullptr;
	m_TestSound2 = nullptr;
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

	m_Camera->SetPosition(0.0f, 2.0f, -10.0f);
	m_Camera->Render();

	strcpy_s(modelFilename, "../CustomEngine/assets/models/floor.txt");

	strcpy_s(textureFilename, "../CustomEngine/assets/textures/grid01.tga");
	strcpy_s(textureFilename1, "../CustomEngine/assets/textures/noise01.tga");
	strcpy_s(textureFilename2, "../CustomEngine/assets/textures/alpha01.tga");

	m_Model = new Model;
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the floor model object.", L"Error", MB_OK);
		return false;
	}

	m_DepthShader = new DepthShader;

	result = m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}


	m_XAudio = new XAudio;
	result = m_XAudio->Initialize();
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize XAudio.", L"Error", MB_ICONERROR | MB_OK);
		return false;
	}

	m_TestSound2 = new XAudioSound3D;

	strcpy_s(soundFilename, "../CustomEngine/assets/sounds/sound04.wav");

	result = m_TestSound2->LoadTrack(m_XAudio->GetXAudio2(), soundFilename, 1.f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not load the test 2 sound.", L"Error", MB_ICONERROR | MB_OK);
		return false;
	}

	m_TestSound2->Update3DPosition(0.0f, 0.0f, 0.0f);

	// result = m_TestSound2->PlayTrack();

	return true;
}


void Application::Shutdown()
{
	if (m_TestSound2)
	{
		m_TestSound2->StopTrack();

		m_TestSound2->ReleaseTrack();
		delete m_TestSound2;
		m_TestSound2 = nullptr;
	}


	if (m_XAudio)
	{
		m_XAudio->Shutdown();
		delete m_XAudio;
		m_XAudio = nullptr;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	if (m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = nullptr;
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

	result = SoundProcessing();
	if (!result)
	{
		return false;
	}

	// result = RenderSceneToTexture(rotation);
	// if (!result)
	// {
	// 	return false;
	// }

	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}


bool Application::SoundProcessing()
{
	bool result;

	result = m_XAudio->Frame(m_TestSound2->GetEmitter(), m_TestSound2->GetSourceVoice());
	if (!result)
	{
		return false;
	}

	return true;
}


/*
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
*/

bool Application::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	XMFLOAT3 cameraPosition, modelPosition;
	double angle;
	float pi, rotationY;
	bool result;

	m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Model->Render(m_Direct3D->GetDeviceContext());
	result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	m_Direct3D->EndScene();

	return true;
}