#include "headers/application.h"

int Application::scene = 0;

Application::Application()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_TextureShader = nullptr;
    m_Cube = nullptr;
    m_IcoSphere = nullptr;
    m_RenderTexture = nullptr;
    m_RenderTexture2 = nullptr;
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

    m_Cube = new Model;

    strcpy_s(modelFilename, "../CustomEngine/assets/models/cube.txt");

    strcpy_s(textureFilename, "../CustomEngine/assets/textures/palestine.tga");
    strcpy_s(textureFilename1, "../CustomEngine/assets/textures/ground01.tga");
    strcpy_s(textureFilename2, "../CustomEngine/assets/textures/alpha01.tga");

    result = m_Cube->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the cube object.", L"Error", MB_OK);
        return false;
    }

    m_IcoSphere = new Model;
    strcpy_s(modelFilename, "../CustomEngine/assets/models/ico_sphere.txt");
    result = m_IcoSphere->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the cube object.", L"Error", MB_OK);
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

    m_RenderTexture2 = new RenderTexture;
    result = m_RenderTexture2->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH, 0);
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

    m_fadeInTime = 10.f;

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

    if (m_RenderTexture2)
    {
        m_RenderTexture2->Shutdown();
        delete m_RenderTexture2;
        m_RenderTexture2 = nullptr;
    }

    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = nullptr;
    }

    if (m_Cube)
    {
        m_Cube->Shutdown();
        delete m_Cube;
        m_Cube = nullptr;
    }

    if (m_IcoSphere)
    {
        m_IcoSphere->Shutdown();
        delete m_IcoSphere;
        m_IcoSphere = nullptr;
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
    float fadePercentage = 1.f;
    bool result = false;
    bool keyPressed = false;

    m_Timer->Frame();

    if (Input->IsEscapePressed())
    {
        return false;
    }

    frameTime = m_Timer->GetTime();
    m_accumulatedTime += frameTime;

    if (!isSceneChanging && previousScene != scenes[scene])
    {
        isSceneChanging = true;
        fadeTimer = 0.f;
        m_accumulatedTime = 0.f;
        previousScene = scenes[scene];
    }

    if (isSceneChanging)
    {
        if (m_accumulatedTime < m_fadeInTime)
        {
            fadePercentage = m_accumulatedTime / m_fadeInTime;
        }

        else
        {
            isSceneChanging = false;
            fadePercentage = 1.0f;
        }
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

    keyPressed = Input->IsBPressed();
    if (keyPressed && scenes[Application::scene] == 0)
    {
        Application::scene = 1;
    }

    else
    {
        Application::scene = 0;
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

    if(scenes[scene] == 0)
    {
        m_Cube->Render(m_Direct3D->GetDeviceContext());
        result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Cube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Cube->GetTexture(0));
        if (!result)
        {
            return false;
        }
    }
    
    m_RenderTexture2->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_RenderTexture2->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_RenderTexture2->GetProjectionMatrix(projectionMatrix);

    worldMatrix = XMMatrixMultiply(XMMatrixRotationY(rotation), XMMatrixTranslation(translationX, translationY, 0.f));

    if (scenes[scene] == 1)
    {
        m_IcoSphere->Render(m_Direct3D->GetDeviceContext());
        result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_IcoSphere->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_IcoSphere->GetTexture(1));
        if (!result)
        {
            return false;
        }
    }

    m_Direct3D->SetBackBufferRenderTarget();
    m_Direct3D->ResetViewport();

    return true;
}


bool Application::Render(float fadePercentage)
{
    XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
    bool result;

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    if (scenes[scene] == 0)
    {
        m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

        m_Direct3D->GetWorldMatrix(worldMatrix);
        m_Camera->GetViewMatrix(viewMatrix);
        m_Direct3D->GetOrthoMatrix(orthoMatrix);

        m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());
        result = m_FadeShader->Render(m_Direct3D->GetDeviceContext(), m_Cube->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView(), fadePercentage);
        if (!result)
        {
            return false;
        }

        m_Direct3D->EndScene();
    }

    else
    {
        m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

        m_Direct3D->GetWorldMatrix(worldMatrix);
        m_Camera->GetViewMatrix(viewMatrix);
        m_Direct3D->GetOrthoMatrix(orthoMatrix);

        m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());
        result = m_FadeShader->Render(m_Direct3D->GetDeviceContext(), m_IcoSphere->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture2->GetShaderResourceView(), fadePercentage);
        if (!result)
        {
            return false;
        }

        m_Direct3D->EndScene();
    }

    return true;
}