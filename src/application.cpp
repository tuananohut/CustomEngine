#include "headers/application.h"


Application::Application()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_TextureShader = nullptr;
    m_Model = nullptr;
    m_RenderTexture = nullptr;
    m_FullScreenWindow = nullptr;
    m_FadeShader = nullptr;
    m_Timer = nullptr;
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
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    m_Camera = new Camera;

    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
    m_Camera->Render();

    m_Model = new Model;

    strcpy_s(modelFilename, "../CustomEngine/assets/models/cube.txt");

    strcpy_s(textureFilename, "../CustomEngine/assets/textures/palestine.tga");
    strcpy_s(textureFilename1, "../CustomEngine/assets/textures/noise01.tga");
    strcpy_s(textureFilename2, "../CustomEngine/assets/textures/alpha01.tga");

    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
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
    result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH, 0);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the render texture object.", L"Error", MB_OK);
        return false;
    }

    m_FullScreenWindow = new OrthoWindow;
    result = m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
        return false;
    }

    m_FadeShader = new FadeShader;
    result = m_FadeShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the fade shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_Timer = new Timer;
    result = m_Timer->Initialize();
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_accumulatedTime = 0.f;

    m_fadeInTime = 5.f;

    return true;
}


void Application::Shutdown()
{
    if (m_FadeShader)
    {
        m_FadeShader->Shutdown();
        delete m_FadeShader;
        m_FadeShader = nullptr;
    }

    if (m_FullScreenWindow)
    {
        m_FullScreenWindow->Shutdown();
        delete m_FullScreenWindow;
        m_FullScreenWindow = nullptr;
    }

    if (m_RenderTexture)
    {
        m_RenderTexture->Shutdown();
        delete m_RenderTexture;
        m_RenderTexture = nullptr;
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

    if (m_Timer)
    {
        delete m_Timer;
        m_Timer = nullptr;
    }
}


bool Application::Frame(Input* Input)
{
    static float rotation = 0.0f;
    static float translationX = 0.f;
    static float translationY = 0.f;
    float frameTime;
    float fadePercentage;
    bool result = false;
    bool keyPressed = false;

    m_Timer->Frame();

    if (Input->IsEscapePressed())
    {
        return false;
    }

    frameTime = m_Timer->GetTime();
    m_accumulatedTime += frameTime * 20.f;

    if (m_accumulatedTime < m_fadeInTime)
    {
        fadePercentage = m_accumulatedTime / m_fadeInTime;
    }

    else
    {
        fadePercentage = 1.f;
    }

    keyPressed = Input->IsRightArrowPressed();
    if (keyPressed && translationX <= 5)
    {
        // For x axis 
        translationX += 25.f * m_Timer->GetTime();
    }

    keyPressed = Input->IsLeftArrowPressed();
    if (keyPressed && translationX >= -5)
    {
        // For x axis 
        translationX -= 25.f * m_Timer->GetTime();
    }

    keyPressed = Input->IsDownArrowPressed();
    if (keyPressed && translationY >= -2.5)
    {
        // For y axis 
        translationY -= 5.f * m_Timer->GetTime();
    }

    keyPressed = Input->IsUpArrowPressed();
    if (keyPressed && translationY <= 2.5)
    {
        // For y axis 
        translationY += 5.f * m_Timer->GetTime();
    }


    rotation -= 0.0174532925f * 0.25f;
    if (rotation < 0.0f)
    {
        rotation += 360.0f;
    }

    result = RenderSceneToTexture(rotation, translationX, translationY, result);
    if (!result)
    {
        return false;
    }    

    result = Render(fadePercentage);
    if (!result)
    {
        return false;
    }

    return true;
}


bool Application::RenderSceneToTexture(float rotation, float translationX, float translationY, bool blur)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_RenderTexture->GetProjectionMatrix(projectionMatrix);
    
    worldMatrix = XMMatrixMultiply(XMMatrixRotationY(rotation), XMMatrixTranslation(translationX, translationY, 0.f));

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


bool Application::Render(float fadePercentage)
{
    XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
    bool result;

    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());
    result = m_FadeShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView(), fadePercentage);
    if (!result)
    {
        return false;
    }

    m_Direct3D->EndScene();

    return true;
}