#include "headers/application.h"

Application::Application()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_Model = nullptr;
    m_RenderTexture = nullptr;
    m_GlowTexture = nullptr;
    m_FullScreenWindow = nullptr;
    m_TextureShader = nullptr;
    m_BlurShader = nullptr;
    m_Blur = nullptr;
    m_GlowShader = nullptr;
}

Application::Application(const Application& other) {}

Application::~Application() {}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    char modelFilename[128];
    char textureFilename[128];
    char glowMapFilename[128];
    char textureFilename1[128];
    int downSampleWidth, downSampleHeight;
    bool result;

    m_Direct3D = new D3D;

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    m_Camera = new Camera;

    m_Camera->SetPosition(0.f, 0.f, -5.0f);
    m_Camera->Render();
    m_Camera->RenderBaseViewMatrix();

    m_Model = new Model;

    strcpy_s(modelFilename, "assets/models/black.txt");
    strcpy_s(textureFilename, "assets/textures/stars.tga");
    strcpy_s(glowMapFilename, "assets/textures/tr.tga");
    strcpy_s(textureFilename1, "assets/textures/stone01.tga");

    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, glowMapFilename, textureFilename1);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
        return false;
    }

    m_RenderTexture = new RenderTexture;
    result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the render texture object.", L"Error", MB_OK);
        return false;
    }

    m_GlowTexture = new RenderTexture;
    result = m_GlowTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the glow render texture object.", L"Error", MB_OK);
        return false;
    }

    m_FullScreenWindow = new OrthoWindow;
    result = m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the full screen window object.", L"Error", MB_OK);
        return false;
    }

    m_TextureShader = new TextureShader;
    result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
        return false;
    }

    m_BlurShader = new BlurShader;
    result = m_BlurShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the blur shader object.", L"Error", MB_OK);
        return false;
    }

    downSampleWidth = screenWidth / 2;
    downSampleHeight = screenHeight / 2;

    m_Blur = new Blur;
    result = m_Blur->Initialize(m_Direct3D, downSampleWidth, downSampleHeight, SCREEN_NEAR, SCREEN_DEPTH, screenWidth, screenHeight);
    if (!result)
    { 
        MessageBox(hwnd, L"Could not initialize the blur object.", L"Error", MB_OK);
        return false;
    }

    m_GlowShader = new GlowShader;
    result = m_GlowShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the glow shader object.", L"Error", MB_OK);
        return false;
    }

    return true;
}

void Application::Shutdown()
{
    if (m_GlowShader)
    {
        m_GlowShader->Shutdown();
        delete m_GlowShader;
        m_GlowShader = nullptr;
    }

    if (m_Blur)
    {
        m_Blur->Shutdown();
        delete m_Blur;
        m_Blur = nullptr;
    }

    if (m_BlurShader)
    {
        m_BlurShader->Shutdown();
        delete m_BlurShader;
        m_BlurShader = nullptr;
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

    if (m_FullScreenWindow)
    {
        m_FullScreenWindow->Shutdown();
        delete m_FullScreenWindow;
        m_FullScreenWindow = nullptr;
    }

    if (m_GlowTexture)
    {
        m_GlowTexture->Shutdown();
        delete m_GlowTexture;
        m_GlowTexture = nullptr;
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
    static float rotation = 0;
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

    result = RenderGlowToTexture(rotation);
    if (!result)
    {
        return false;
    }

    result = m_Blur->BlurTexture(m_Direct3D, m_Camera, m_GlowTexture, m_TextureShader, m_BlurShader);
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

bool Application::RenderGlowToTexture(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    m_GlowTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_GlowTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    worldMatrix = XMMatrixRotationY(rotation);

    m_Model->Render(m_Direct3D->GetDeviceContext());
    result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(1));
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
    XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
    float glowValue;
    bool result;

    m_Direct3D->BeginScene(0.0f, 0.5f, 0.8f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetBaseViewMatrix(baseViewMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    m_Direct3D->TurnZBufferOff();

    glowValue = 5.f;

    m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());
    result = m_GlowShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView(), m_GlowTexture->GetShaderResourceView(), glowValue);
    if (!result)
    {
        return false;
    }

    m_Direct3D->TurnZBufferOn();

    m_Direct3D->EndScene();

    return true;
}
