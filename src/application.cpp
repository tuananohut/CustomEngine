#include "headers/application.h"

Application::Application()
{
	m_Direct3D = NULL;
	m_Camera = NULL;
	m_Model = NULL;
	m_ColorShader = NULL;
	m_TextureShader = NULL;
	m_LightShader = NULL;
	m_Light = NULL;
}

Application::Application(const Application& other)
{

}


Application::~Application()
{

}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128];
	char textureFilename[128];
	bool result;

	m_Direct3D = new D3D;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_Camera = new Camera;

	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	strcpy_s(modelFilename, "src/assets/models/cube.txt");

	strcpy_s(textureFilename, "src/assets/shaders/palestine.tga");

	m_Model = new Model;

	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	
	// m_ColorShader = new ColorShader;
	// 
	// result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	// if(!result)
	// {
	// 	MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
	// 	return false;
	// }
	// 
	// 
	// m_TextureShader = new TextureShader;
	//   
	// result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	// if (!result)
	// {
	// 	MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
	// 	return false;
	// }


	m_LightShader = new LightShader;

	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	m_Light = new Light;

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

void Application::Shutdown()
{
	if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = NULL;
	}
	
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = NULL;
	}
	
	if(m_Light)
	{
		delete m_Light;
		m_Light = NULL;
	}

	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = NULL;
	}

	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = NULL;
	}

	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = NULL;
	}

	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = NULL;
	}

	return;
}

bool Application::Frame()
{
	static float rotation = 0.0f;
	bool result;

	rotation -= 0.0174532925f * 0.25f;
	if(rotation < 0.0f)
	{
		rotation += 360.0f;
	}

	result = Render(rotation);
	if(!result)
	{

		return false;
	}

	return true;
}

bool Application::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	XMMATRIX rotationX = XMMatrixRotationX(rotation);
	XMMATRIX rotationZ = XMMatrixRotationZ(rotation);

	worldMatrix = XMMatrixMultiply(rotationX, rotationZ);

	m_Model->Render(m_Direct3D->GetDeviceContext());
	 
	// result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	// if(!result)
	// {
	// 	return false;
	// }
	// 
	// result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	// if(!result)
	// {
	// 	return false;
	// }

	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if(!result)	
	{
		return false;
	}

	m_Direct3D->EndScene();

	return true;
}