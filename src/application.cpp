#include "headers/application.h"

Application::Application()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_TextureShader = nullptr;
	m_FloorModel = nullptr;
	m_BillboardModel = nullptr;
	m_Position = nullptr;
	m_Timer = nullptr;

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
	
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->Render();
	
	m_TextureShader = new TextureShader;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	strcpy_s(modelFilename, "../CustomEngine/assets/models/floor.txt");
	
	strcpy_s(textureFilename, "../CustomEngine/assets/textures/grid01.tga");
	strcpy_s(textureFilename1, "../CustomEngine/assets/textures/noise01.tga");
	strcpy_s(textureFilename2, "../CustomEngine/assets/textures/alpha01.tga");

	m_FloorModel = new Model;
	result = m_FloorModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the floor model object.", L"Error", MB_OK);
		return false;
	}
	
	strcpy_s(modelFilename, "../CustomEngine/assets/models/square.txt");
	strcpy_s(textureFilename, "../CustomEngine/assets/textures/stone01.tga");

	m_BillboardModel = new Model;
	result = m_BillboardModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the billboard model object.", L"Error", MB_OK | MB_ICONERROR);
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

	result = m_TestSound2->PlayTrack();

	m_Position = new Position;
	m_Position->SetPosition(0.f, 1.5f, -11.f);

	m_Timer = new Timer;
	m_Timer->Initialize();

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

	if (m_Position)
	{
		delete m_Position;
		m_Position = nullptr;
	}

	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = nullptr;
	}

	if (m_BillboardModel)
	{
		m_BillboardModel->Shutdown();
		delete m_BillboardModel;
		m_BillboardModel = nullptr;
	}

	if (m_FloorModel)
	{
		m_FloorModel->Shutdown();
		delete m_FloorModel;
		m_FloorModel = nullptr;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = nullptr;
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
	
	float positionX, positionY, positionZ; 
	bool result, keyDown;

	m_Timer->Frame();

	if (Input->IsEscapePressed())
	{
		return false;
	}

	m_Position->SetFrameTime(m_Timer->GetTime());

	keyDown = Input->IsLeftArrowPressed();
	m_Position->MoveLeft(keyDown);
	
	keyDown = Input->IsRightArrowPressed();
	m_Position->MoveRight(keyDown);
	
	m_Position->GetPosition(positionX, positionY, positionZ);

	m_Camera->SetPosition(positionX, positionY, positionZ);
	m_Camera->Render();

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

	m_Direct3D->EnableAlphaBlending();

	m_FloorModel->Render(m_Direct3D->GetDeviceContext());
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_FloorModel->GetTexture(0));
	if (!result)
	{
		return false;
	}

	cameraPosition = m_Camera->GetPosition();

	modelPosition.x = 0.f;
	modelPosition.y = 1.5f;
	modelPosition.z = 0.f;

	pi = 3.14159265358979323846f;
	angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.y - cameraPosition.z) * (180.0f / pi);

	rotationY = (float)angle * 0.0174532925f;

	worldMatrix = XMMatrixRotationY(rotationY);

	translateMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

	m_BillboardModel->Render(m_Direct3D->GetDeviceContext());
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_BillboardModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_BillboardModel->GetTexture(0));
	if (!result)
	{
		return false;
	}

	m_Direct3D->DisableAlphaBlending();

	m_Direct3D->EndScene();

	return true;
}