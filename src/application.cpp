#include "headers/application.h"

Application::Application()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_FontShader = nullptr;
    m_Font = nullptr;
    m_Text1 = nullptr;
    m_Text2 = nullptr;
    m_Fps = nullptr;
}

Application::Application(const Application& other) {}

Application::~Application() {}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    char modelFilename[128];
    char textureFilename[128];
    char glowMapFilename[128];
    char textureFilename1[128];
    char testString1[32];
    char testString2[32];
    bool result;

    m_Direct3D = new D3D;

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    m_Camera = new Camera;

    m_Camera->SetPosition(0.f, 0.f, -10.0f);
    m_Camera->Render();
    /*
    m_Model = new Model;

    strcpy_s(modelFilename, "assets/models/cube.txt");
    strcpy_s(textureFilename, "assets/textures/black.tga");
    strcpy_s(glowMapFilename, "assets/textures/glowmap001.tga");
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
    */

    m_FontShader = new FontShader;
    result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
        return false;
    }

    m_Font = new Font;
    result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
        return false;
    }

    strcpy_s(testString1, "Fps: 0");
    strcpy_s(testString2, "Palestine");


    m_Text1 = new Text;
    result = m_Text1->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, testString1, screenWidth / 2, screenHeight / 2 - 64, 1.f, 0.f, 0.f);
    if (!result)
    {
        return false;
    }

    m_Text2 = new Text;
    result = m_Text2->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, testString2, screenWidth / 2, screenHeight / 2 + 64, 1.f, 1.f, 1.f);
    if (!result)
    {
        return false;
    }

    m_Fps = new Fps;
    m_Fps->Initialize();

    m_previousFps = -1;

    return true;
}

void Application::Shutdown()
{
    if (m_Fps)
    {
        delete m_Fps;
        m_Fps = nullptr;
    }

    if (m_Text1)
    {
        m_Text1->Shutdown();
        delete m_Text1;
        m_Text1 = nullptr;
    }

    if (m_Text2)
    {
        m_Text2->Shutdown();
        delete m_Text2;
        m_Text2 = nullptr;
    }

    if (m_Font)
    {
        m_Font->Shutdown();
        delete m_Font;
        m_Font = nullptr;
    }

    if (m_FontShader)
    {
        m_FontShader->Shutdown();
        delete m_FontShader;
        m_FontShader = nullptr;
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

    result = UpdateFps();
    if (!result)
    {
        return false;
    }

    rotation -= 0.0174532925f * 0.25f;
    if (rotation < 0.0f)
    {
        rotation += 360.0f;
    }

    result = Render(rotation);
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

    // worldMatrix = XMMatrixMultiply(XMMatrixScaling(0.5f, 0.5f, 0.5f), XMMatrixRotationX(rotation));
    // worldMatrix = XMMatrixMultiply(XMMatrixScaling(0.1f, 0.1f, 0.1f), XMMatrixTranslation(0.f, -1.f, 0.f));
    worldMatrix = XMMatrixRotationX(rotation);

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

    worldMatrix = XMMatrixRotationX(rotation);

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

*/

bool Application::Render(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
    bool result;

    m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    m_Direct3D->TurnZBufferOff();
    m_Direct3D->EnableAlphaBlending();

    m_Text1->Render(m_Direct3D->GetDeviceContext());
    result = m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_Text1->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Font->GetTexture(), m_Text1->GetPixelColor());
    if (!result)
    {
        return false;
    }

    m_Text2->Render(m_Direct3D->GetDeviceContext());
    result = m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_Text2->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Font->GetTexture(), m_Text2->GetPixelColor());
    if (!result)
    {
        return false;
    }


    m_Direct3D->TurnZBufferOn();
    m_Direct3D->DisableAlphaBlending();

    m_Direct3D->EndScene();

    return true;
}

bool Application::UpdateFps()
{
    int fps;
    char tempString[16], finalString[16];
    float red, green, blue;
    bool result;

    m_Fps->Frame();

    fps = m_Fps->GetFps();

    if (m_previousFps == fps)
    {
        return true;
    }

    m_previousFps = fps;

    if (fps > 99999)
    {
        fps = 99999;
    }

    sprintf_s(tempString, "%d", fps);

    strcpy_s(finalString, "Fps: ");
    strcat_s(finalString, tempString);

    if (fps >= 60)
    {
        red = 0.f;
        green = 1.f;
        blue = 0.f;
    }

    if (fps < 60)
    {
        red = 1.f;
        green = 1.f;
        blue = 0.f;
    }

    if (fps < 30)
    {
        red = 1.f; 
        green = 0.f;
        blue = 0.f;
    }

    result = m_Text1->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 10, red, green, blue);
    if (!result)
    {
        return false;
    }

    return true;
}