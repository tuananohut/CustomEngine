#include "headers/application.h"

Application::Application()
{
  m_Direct3D = nullptr;
  m_Camera = nullptr;
  m_Model = nullptr;
  m_ColorShader = nullptr;
  m_TextureShader = nullptr;
  m_Bitmap = nullptr;
  m_LightShader = nullptr;
  m_Lights = nullptr;
  m_Sprite = nullptr;
  m_Timer = nullptr;
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
  char bitmapFilename[128];
  char spriteFilename[128];
  bool result;

  m_Direct3D = new D3D;

  result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
  if(!result)
    {
      MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
      return false;
    }

  m_Camera = new Camera;

  m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
  // m_Camera->SetRotation(30.0f, 0.0f, 0.0f);
  m_Camera->Render();

  m_TextureShader = new TextureShader;

  result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
  if(!result)
    {
      MessageBox(hwnd, L"Could not initialize the texure shader object.", L"Error", MB_OK);
      return false;
    }

  strcpy_s(spriteFilename, "src/assets/sprites/sprite_data_01.txt");

  m_Sprite = new Sprite;

  result = m_Sprite->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, spriteFilename, 50, 50);
  if(!result)
    {
      MessageBox(hwnd, L"Bu sprite baþlatýlamýyor.", L"Neden bilmiyorum ama", MB_OK);
      return false;
    }

  m_Timer = new Timer;

  result = m_Timer->Initialize();
  if(!result)
    {
      return false;
    }
  
  return true;
}

void Application::Shutdown()
{
  if(m_Timer)
    {
      delete m_Timer;
      m_Timer = nullptr;
    }

  if(m_Sprite)
    {
      m_Sprite->Shutdown();
      delete m_Sprite;
      m_Sprite = nullptr;
    }
  
  if(m_Bitmap)
    {
      m_Bitmap->Shutdown();
      delete m_Bitmap;
      m_Bitmap = nullptr;
    }
  
  if(m_Lights)
    {
      delete[] m_Lights;
      m_Lights = nullptr;
    }

  if(m_ColorShader)
    {
      m_ColorShader->Shutdown();
      delete m_ColorShader;
      m_ColorShader = nullptr;
    }
	
  if(m_TextureShader)
    {
      m_TextureShader->Shutdown();
      delete m_TextureShader;
      m_TextureShader = nullptr;
    }

  if(m_LightShader)
    {
      m_LightShader->Shutdown();
      delete m_LightShader;
      m_LightShader = nullptr;
    }

  if(m_Model)
    {
      m_Model->Shutdown();
      delete m_Model;
      m_Model = nullptr;
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
  static float rotation = 0.0f;
  static float translationX = 0.0f;
  static float translationZ = 0.0f;
  static float speedX = 0.1f;
  static float speedZ = 0.1f;
  float frameTime;
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

  m_Timer->Frame();

  frameTime = m_Timer->GetTime();

  m_Sprite->Update(frameTime);
  
  result = Render(rotation, translationX, translationZ);
  if(!result)
    {

      return false;
    }

  return true;
}

bool Application::Render(float rotation, float translationX, float translationZ)
{
  XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotateMatrix, translateMatrix, scaleMatrix, srMatrix, orthoMatrix;
  XMFLOAT4 diffuseColor[4], lightPosition[4];
  int i;
  bool result;

  m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

  m_Camera->Render();

  m_Direct3D->GetWorldMatrix(worldMatrix);
  m_Camera->GetViewMatrix(viewMatrix);
  m_Direct3D->GetOrthoMatrix(orthoMatrix);

  m_Direct3D->TurnZBufferOff();

  result = m_Sprite->Render(m_Direct3D->GetDeviceContext());
  if(!result)
    {
      return false;
    }

  result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Sprite->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Sprite->GetTexture());
  if(!result)
    {
      return false;
    }

  m_Direct3D->TurnZBufferOn();
  
  m_Direct3D->EndScene();

  return true;
}
