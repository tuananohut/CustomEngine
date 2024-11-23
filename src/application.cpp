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

    m_Camera->SetPosition(-8.0f, 7.0f, 8.0f);
    m_Camera->SetRotation(35.0f, 135.0f, 0.0f);
    m_Camera->Render();

    m_CubeModel = new Model;

    strcpy_s(modelFilename, "../CustomEngine/assets/models/cube.txt");
    strcpy_s(textureFilename, "../CustomEngine/assets/textures/stone01.tga");
    strcpy_s(textureFilename1, "../CustomEngine/assets/textures/stone01.tga");
    strcpy_s(textureFilename2, "../CustomEngine/assets/textures/stone01.tga");

    result = m_CubeModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
        return false;
    }

    m_SphereModel = new Model;

    strcpy_s(modelFilename, "../CustomEngine/assets/models/sphere.txt");
    strcpy_s(textureFilename, "../CustomEngine/assets/textures/ice01.tga");

    result = m_SphereModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK);
        return false;
    }

    m_GroundModel = new Model;

    strcpy_s(modelFilename, "../CustomEngine/assets/models/plane01.txt");
    strcpy_s(textureFilename, "../CustomEngine/assets/textures/metal001.tga");

    result = m_GroundModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
        return false;
    }

    m_Light = new Light;

    m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.f);
    m_Light->SetDiffuseColor(1.f, 1.f, 1.f, 1.f);
    m_Light->GenerateOrthoMatrix(20.f, SHADOWMAP_NEAR, SHADOWMAP_DEPTH);

    m_RenderTexture = new RenderTexture;
    result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SHADOWMAP_DEPTH, SHADOWMAP_NEAR, 1);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the render texture object.", L"Error", MB_OK);
        return false;
    }

    m_DepthShader = new DepthShader;
    result = m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
        return false;
    }

    m_ShadowShader = new ShadowShader;
    result = m_ShadowShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
        return false;
    }

    m_shadowMapBias = 0.0022f;
    
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

    if (m_GroundModel)
    {
        m_GroundModel->Shutdown();
        delete m_GroundModel;
        m_GroundModel = nullptr;
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
    static float lightAngle = 270.f;
    float radians;
    float frameTime;
    static float lightPosX = 9.f;
    bool result;
    XMMATRIX translateMatrix = {}, lightViewMatrix = {}, lightOrthoMatrix = {};

    if (Input->IsEscapePressed())
    {
        return false;
    }

    frameTime = 10.f;

    lightPosX -= 0.003f * frameTime;

    lightAngle -= 0.03f * frameTime;
    if (lightAngle < 90.f)
    {
        lightAngle = 270.f;

        lightPosX = 9.f;
    }

    radians = lightAngle * 0.0174532925f;

    m_Light->SetDirection(sinf(radians), cosf(radians), 0.f);

    m_Light->SetPosition(lightPosX, 8.0f, -0.1f);
    m_Light->SetLookAt(-lightPosX, 0.f, 0.f);
    m_Light->GenerateViewMatrix();

    result = RenderDepthToTexture(translateMatrix, lightViewMatrix, lightOrthoMatrix, m_RenderTexture, m_Light);
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

bool Application::RenderDepthToTexture(XMMATRIX translateMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightOrthoMatrix, RenderTexture* m_RenderTexture, Light* m_Light)
{
    bool result;

    m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    translateMatrix = XMMatrixTranslation(-2.0f, 2.0f, 0.0f);

    m_CubeModel->Render(m_Direct3D->GetDeviceContext());

    result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), translateMatrix, lightViewMatrix, lightOrthoMatrix);
    if (!result)
    {
        return false;
    }

    translateMatrix = XMMatrixTranslation(2.0f, 2.0f, 0.0f);

    m_SphereModel->Render(m_Direct3D->GetDeviceContext());
    result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), translateMatrix, lightViewMatrix, lightOrthoMatrix);
    if (!result)
    {
        return false;
    }

    translateMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), translateMatrix, lightViewMatrix, lightOrthoMatrix);
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
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightOrthoMatrix, lightViewMatrix2, lightOrthoMatrix2, lightViewMatrix3, lightOrthoMatrix3;
    bool result;

    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    worldMatrix = XMMatrixTranslation(-2.0f, 2.0f, 0.0f);

    m_CubeModel->Render(m_Direct3D->GetDeviceContext());

    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_CubeModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    worldMatrix = XMMatrixTranslation(2.0f, 2.0f, 0.0f);

    m_SphereModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_SphereModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    worldMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_GroundModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    m_Direct3D->EndScene();

    return true;
}
