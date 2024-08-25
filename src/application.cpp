#include "headers/application.h"

Application::Application()
{
  m_Direct3D = nullptr;
  m_Camera = nullptr;
  m_Model = nullptr;
  m_Model1 = nullptr;
  m_Model2 = nullptr;
  m_ShaderManager = nullptr;
  m_Light = nullptr;
  m_ModelList = nullptr;
  m_Timer = nullptr;
  m_Position = nullptr;
  m_Frustum = nullptr;
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
  char textureFilename3[128];

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
  m_Camera->SetRotation(0.0f, 0.0f, 0.0f);
  m_Camera->Render();
  m_Camera->GetViewMatrix(m_baseViewMatrix);

  strcpy_s(modelFilename, "../CustomEngine/src/assets/models/cube.txt");
  

  strcpy_s(textureFilename1, "../CustomEngine/src/assets/shaders/stone01.tga");
  strcpy_s(textureFilename2, "../CustomEngine/src/assets/shaders/normal01.tga");
  strcpy_s(textureFilename3, "../CustomEngine/src/assets/shaders/palestine.tga");

  m_Model = new Model;

  result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename1, textureFilename2, textureFilename3);
  if (!result)
  {
      return false;
  }

  m_Model1 = new Model;

  result = m_Model1->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename1, textureFilename2, textureFilename3);
  if (!result)
  {
      return false;
  }

  m_Model2 = new Model;

  result = m_Model2->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename1, textureFilename2, textureFilename3);
  if (!result)
  {
      return false;
  }

  m_Light = new Light;

  m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
  m_Light->SetDirection(0.0f, 0.0f, 1.0f);

  m_ShaderManager = new ShaderManager;

  result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd);
  if (!result)
  {
      return false;
  }

  m_ModelList = new ModelList;
  m_ModelList->Initialize(25);

  m_Timer = new Timer;
  result = m_Timer->Initialize();
  if (!result)
  {
      return false;
  }

  m_Position = new Position;

  m_Frustum = new Frustum;

  return true;
}

void Application::Shutdown()
{
    if (m_Frustum)
    {
        delete m_Frustum;
        m_Frustum = nullptr;
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

    if (m_ModelList)
    {
        m_ModelList->Shutdown();
        delete m_ModelList;
        m_ModelList = nullptr;
    }

    if(m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = nullptr;
    }
    
    if (m_Model1)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = nullptr;
    }

    if (m_Model2)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = nullptr;
    }

    if(m_ShaderManager)
    {
        m_ShaderManager->Shutdown();
        delete m_ShaderManager;
        m_ShaderManager = nullptr;
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
}

bool Application::Frame(Input* Input)
{
  static float rotation = 360.0f;

  float rotationY;
  bool result;
  bool keyDown;

  m_Timer->Frame();

  if (Input->IsEscapePressed())
  {
      return false;
  }

  m_Position->SetFrameTime(m_Timer->GetTime());

  keyDown = Input->IsLeftArrowPressed();
  m_Position->TurnLeft(keyDown);

  keyDown = Input->IsRightArrowPressed();
  m_Position->TurnRight(keyDown);

  m_Position->GetRotation(rotationY);

  m_Camera->SetRotation(0.0f, rotationY, 0.0f);
  m_Camera->Render();

  rotation -= 0.0174532925f * 0.25f;
  if (rotation < 0.0f)
  {
      rotation += 360.f;
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
  XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotateMatrixX, rotateMatrixY, rotateMatrixZ, rotateMatrix, translateMatrix, orthoMatrix;
  XMFLOAT4 diffuseColor[4], lightPosition[4];
  float positionX, positionY, positionZ, radius;
  int modelCount, renderCount;
  bool renderModel;
  int i;
  bool result;

  m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

  m_Camera->Render();

  m_Direct3D->GetWorldMatrix(worldMatrix);
  m_Camera->GetViewMatrix(viewMatrix);
  m_Direct3D->GetProjectionMatrix(projectionMatrix);
  m_Direct3D->GetOrthoMatrix(orthoMatrix);

  m_Frustum->ConstructFrustum(viewMatrix, projectionMatrix, SCREEN_DEPTH);

  modelCount = m_ModelList->GetModelCount();

  renderCount = 0;

  for (i = 0; i < modelCount; i += 1)
  {
      m_ModelList->GetData(i, positionX, positionY, positionZ);

      radius = 1.0f;

      renderModel = m_Frustum->CheckCube(positionX, positionY, positionZ, radius);
      if (renderModel)
      {
          rotateMatrixX = XMMatrixRotationX(rotation);
          rotateMatrixY = XMMatrixRotationY(rotation);
          rotateMatrixZ = XMMatrixRotationZ(rotation);

          worldMatrix = XMMatrixMultiply(rotateMatrixX, rotateMatrixY);
          worldMatrix = XMMatrixMultiply(worldMatrix, rotateMatrixZ);

          worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(positionX, positionY, positionZ));
          
          m_Model->Render(m_Direct3D->GetDeviceContext());
          result = m_ShaderManager->RenderNormalMapShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
              m_Model->GetTexture(0), m_Model->GetTexture(1), m_Light->GetDirection(), m_Light->GetDiffuseColor());
          if (!result)
          {
              return false;
          }
      }
  }

  m_Direct3D->TurnZBufferOff();
  m_Direct3D->EnableAlphaBlending();

  m_Direct3D->GetWorldMatrix(worldMatrix);

  m_Direct3D->EndScene();

  return true;
}
