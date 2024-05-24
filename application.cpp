#include "headers/application.h"

Application::Application()
{
	m_Direct3D = NULL;
	m_Camera = NULL;
	m_Model = NULL;
	m_ColorShader = NULL;
	m_TextureShader = NULL;
	m_LightShader = NULL;
	m_Lights = NULL;
	// m_Light = NULL;
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

	m_Camera->SetPosition(0.0f, 2.0f, -15.0f);
    // m_Camera->SetRotation(30.0f, 0.0f, 0.0f);
	m_Camera->Render();

	strcpy_s(modelFilename, "src/assets/models/plane.txt");

	strcpy_s(textureFilename, "src/assets/shaders/palestine.tga");

	m_Model = new Model;

	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_LightShader = new LightShader;

	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	m_numLights = 4;

	m_Lights = new Light[m_numLights];

	m_Lights[0].SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_Lights[0].SetPosition(-3.0f, 1.0f, 3.0f);

	m_Lights[1].SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
	m_Lights[1].SetPosition(3.0f, 1.0f, 3.0f);

	m_Lights[2].SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);
	m_Lights[2].SetPosition(-3.0f, 1.0f, -3.0f);

	m_Lights[3].SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Lights[3].SetPosition(3.0f, 1.0f, -3.0f);

	// m_Light = new Light;
	// 
	// m_Light->SetAmbientColor(0.5f, 0.5f, 0.5f, 1.0f);
	// m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	// m_Light->SetDirection(1.0f, 0.0f, 1.0f);
	// m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	// m_Light->SetSpecularPower(32.0f);


	return true;
}

void Application::Shutdown()
{
	if(m_Lights)
	{
		delete[] m_Lights;
		m_Lights = NULL;
	}

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
	
	// if(m_Light)
	// {
	// 	delete m_Light;
	// 	m_Light = NULL;
	// }

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
	static float translationX = 0.0f;
	static float translationZ = 0.0f;
	static float speedX = 0.1f;
	static float speedZ = 0.1f;
	bool result;

	rotation -= 0.0174532925f * 0.25f;
	if(rotation < 0.0f)
	{
		rotation += 360.0f;
	}
	
	// translationX += speedX * 0.5f; 
	// translationZ += speedZ * 0.2f;
	// 
	// if (translationZ > 4.0f || translationZ < 0.0f) 
	// {
	// 	speedZ *= -1.0f;
	// }
	// 
	// if (translationX > 5.0f || translationX < -5.0f) 
	// {
	// 	speedX *= -1.0f;
	// }	

	translationX = 4.f * std::cos(rotation);
	translationZ = 4.f * std::sin(rotation);

	result = Render(rotation, translationX, translationZ);
	if(!result)
	{

		return false;
	}

	return true;
}

bool Application::Render(float rotation, float translationX, float translationZ)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotateMatrix, translateMatrix, scaleMatrix, srMatrix;
	XMFLOAT4 diffuseColor[4], lightPosition[4];
	int i;
	bool result;

	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	for(i = 0; i < m_numLights; i++)
	{
		diffuseColor[i] = m_Lights[i].GetDiffuseColor();
		
		lightPosition[i] = m_Lights[i].GetPosition();
	}

	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), diffuseColor, lightPosition);
	if(!result)
	{
		return false;
	}

	//*** ONE CUBE ROTATES - WATERMELON ***
	XMMATRIX rotationX = XMMatrixRotationX(rotation);
	XMMATRIX rotationY = XMMatrixRotationY(rotation);
	XMMATRIX rotationZ = XMMatrixRotationZ(rotation);
	// 
	// worldMatrix = XMMatrixMultiply(rotationX, rotationZ);
	// 
	// m_Model->Render(m_Direct3D->GetDeviceContext());

	rotateMatrix = XMMatrixMultiply(rotationX, rotationY);
	translateMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	
	worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);

	m_Model->Render(m_Direct3D->GetDeviceContext());

	// result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	// if (!result)
	// {
	// 	return false;
	// }

	// scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	// rotateMatrix = XMMatrixMultiply(rotationX, rotationZ);
	// translateMatrix = XMMatrixTranslation(translationX + 2.5f, 0.0f, translationZ);
	// 
	// srMatrix = XMMatrixMultiply(scaleMatrix, rotateMatrix);
	// worldMatrix = XMMatrixMultiply(srMatrix, translateMatrix);
	// 
	// m_Model->Render(m_Direct3D->GetDeviceContext());
	// 
	// result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	// if(!result)	
	// {
	// 	return false;
	// }

	m_Direct3D->EndScene();

	return true;
}