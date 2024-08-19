#include "headers/application.h"

Application::Application()
{
  m_Direct3D = nullptr;
  m_Camera = nullptr;
  m_Model = nullptr;
  m_NormalMapShader = nullptr;
  m_Light = nullptr;
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
  char textureFilename1[128];
  char textureFilename2[128];

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
  m_Camera->SetRotation(0.0f, 0.0f, 0.0f);
  m_Camera->Render();

  m_NormalMapShader = new NormalMapShader;

  result = m_NormalMapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
  if(!result)
  {
      MessageBox(hwnd, L"Could not initialize the normal map shader object.", L"Error", MB_OK);
      return false;
  }

  strcpy_s(modelFilename, "../CustomEngine/src/assets/models/sphere.txt");

  strcpy_s(textureFilename1, "../CustomEngine/src/assets/shaders/stone01.tga");
  strcpy_s(textureFilename2, "../CustomEngine/src/assets/shaders/normal01.tga");

  m_Model = new Model;

  result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename1, textureFilename2);
  if(!result)
  {
      return false;
  }

  m_Light = new Light;

  m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
  m_Light->SetDirection(0.0f, 0.0f, 1.0f);
 
  return true;
}

void Application::Shutdown()
{
  if(m_Model)
    {
      m_Model->Shutdown();
      delete m_Model;
      m_Model = nullptr;
    }

  if(m_NormalMapShader)
  {
      m_NormalMapShader->Shutdown();
      delete m_NormalMapShader;
      m_NormalMapShader = nullptr;
  }

  if(m_Light)
  {
      delete m_Light;
      m_Light = nullptr;
  }

  if(m_Camera)
    {
      delete m_Camera;
      m_Camera = nullptr;
    }

  if(m_Direct3D)
    {
      m_Direct3D->Shutdown();
      delete m_Direct3D;
      m_Direct3D = nullptr;
    }

  return;
}

bool Application::Frame()
{
  static float rotation = 360.0f;
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
	
    translationX += speedX * 0.5f; 
    translationZ += speedZ * 0.2f;
    
    if (translationZ > 4.0f || translationZ < 0.0f) 
    {
    	speedZ *= -1.0f;
    }
    
    if (translationX > 5.0f || translationX < -5.0f) 
    {
    	speedX *= -1.0f;
    }	

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
  XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotateMatrixX, rotateMatrixZ;
  XMFLOAT4 diffuseColor[4], lightPosition[4];
  int i;
  bool result;

  m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

  m_Camera->Render();

  m_Direct3D->GetWorldMatrix(worldMatrix);
  m_Camera->GetViewMatrix(viewMatrix);
  m_Direct3D->GetProjectionMatrix(projectionMatrix);

  rotateMatrixX = XMMatrixRotationX(rotation);
  rotateMatrixZ = XMMatrixRotationZ(rotation);

  worldMatrix = XMMatrixMultiply(rotateMatrixX, rotateMatrixZ);

  m_Model->Render(m_Direct3D->GetDeviceContext());

  result = m_NormalMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(0), m_Model->GetTexture(1), m_Light->GetDirection(), m_Light->GetDiffuseColor());
  if(!result)
  {
      return false;
  }

  m_Direct3D->EndScene();

  return true;
}
