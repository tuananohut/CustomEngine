#include "headers/application.h"


Application::Application()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_CubeModel = nullptr;
    m_SphereModel = nullptr;
    m_GroundModel = nullptr;
    m_Light = nullptr;
    m_RenderTexture = nullptr;
    m_DepthShader = nullptr;
    m_ShadowShader = nullptr;
}

Application::Application(const Application& other) {}

Application::~Application() {}


bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    char modelFilename[128];
    char soundFilename[128];
    char textureFilename[128];
    char textureFilename1[128];
    char textureFilename2[128];
    bool result;

    m_Direct3D = new D3D;
    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_Camera = new Camera;

    m_Camera->SetPosition(0.0f, 7.0f, -10.0f);
    m_Camera->SetRotation(35.f, 0.f, 0.f);
    m_Camera->Render();

    strcpy_s(modelFilename, "../CustomEngine/assets/models/plane01.txt");
    strcpy_s(textureFilename, "../CustomEngine/assets/textures/metal001.tga");
    strcpy_s(textureFilename1, "../CustomEngine/assets/textures/palestine.tga");
    strcpy_s(textureFilename2, "../CustomEngine/assets/textures/palestine.tga");

    m_GroundModel = new Model;
    result = m_GroundModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    strcpy_s(modelFilename, "../CustomEngine/assets/models/cube.txt");
    strcpy_s(textureFilename, "../CustomEngine/assets/textures/stone01.tga");

    m_CubeModel = new Model;
    result = m_CubeModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    
    strcpy_s(modelFilename, "../CustomEngine/assets/models/sphere.txt");
    strcpy_s(textureFilename, "../CustomEngine/assets/textures/ice01.tga");

    m_SphereModel = new Model;
    result = m_SphereModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    
    m_Light = new Light;

    m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.f);
    m_Light->SetDiffuseColor(1.f, 1.f, 1.f, 1.f);
    m_Light->SetLookAt(0.f, 0.f, 0.f);
    m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

    m_RenderTexture = new RenderTexture;
    result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the render texture object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_DepthShader = new DepthShader;
    result = m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_ShadowShader = new ShadowShader;
    result = m_ShadowShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

     m_shadowMapBias = 0.0022f;
     // m_shadowMapBias = 1.f;

    return true;
}


void Application::Shutdown()
{
    if (m_ShadowShader)
    {
        m_ShadowShader->Shutdown();
        delete m_ShadowShader;
        m_ShadowShader = nullptr;
    }

    if (m_DepthShader)
    {
        m_DepthShader->Shutdown();
        delete m_DepthShader;
        m_DepthShader = nullptr;
    }

    if (m_RenderTexture)
    {
        m_RenderTexture->Shutdown();
        delete m_RenderTexture;
        m_RenderTexture = nullptr;
    }

    if (m_Light)
    {
        delete m_Light;
        m_Light = nullptr;
    }

    if (m_SphereModel)
    {
        m_SphereModel->Shutdown();
        delete m_SphereModel;
        m_SphereModel = nullptr;
    }

    if (m_CubeModel)
    {
        m_CubeModel->Shutdown();
        delete m_CubeModel;
        m_CubeModel = nullptr;
    }

    if (m_GroundModel)
    {
        m_GroundModel->Shutdown();
        delete m_GroundModel;
        m_GroundModel = nullptr;
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
    static float rotation = 0.0f;
    static float lightPositionX = -5.f;
    bool result;

    if (Input->IsEscapePressed())
    {
        return false;
    }

    lightPositionX += 0.05f;
    if (lightPositionX > 5.f)
    {
        lightPositionX = -5.f;
    }

    m_Light->SetPosition(lightPositionX, 8.f, -5.f);
    m_Light->GenerateViewMatrix();

    result = RenderDepthToTexture();
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
*/

/*
bool Application::SoundProcessing()
{
    bool result;

    result = m_XAudio->Frame(m_Sound->GetEmitter(), m_Sound->GetSourceVoice());
    if (!result)
    {
        return false;
    }

    return true;
}
*/

bool Application::RenderDepthToTexture()
{
    XMMATRIX translateMatrix, lightViewMatrix, lightProjectionMatrix;
    bool result;

    m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetProjectionMatrix(lightProjectionMatrix);

    translateMatrix = XMMatrixTranslation(-1.f, 2.f, 0.f);

    m_CubeModel->Render(m_Direct3D->GetDeviceContext());

    result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), translateMatrix, lightViewMatrix, lightProjectionMatrix);
    if (!result)
    {
        return false;
    }

    translateMatrix = XMMatrixTranslation(1.f, 2.f, 0.f);

    m_SphereModel->Render(m_Direct3D->GetDeviceContext());

    result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), translateMatrix, lightViewMatrix, lightProjectionMatrix);
    if (!result)
    {
        return false;
    }

    translateMatrix = XMMatrixTranslation(0.f, 1.f, 0.f);

    m_GroundModel->Render(m_Direct3D->GetDeviceContext());

    result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), translateMatrix, lightViewMatrix, lightProjectionMatrix);
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
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix;
    bool result;

    m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetProjectionMatrix(lightProjectionMatrix);

    worldMatrix = XMMatrixTranslation(-1.f, 2.f, 0.f);

    m_CubeModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_CubeModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    worldMatrix = XMMatrixTranslation(1.f, 2.f, 0.f);

    m_SphereModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_SphereModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    worldMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_GroundModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    m_Direct3D->EndScene();

    return true;
}
