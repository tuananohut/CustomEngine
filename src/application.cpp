#include "headers/application.h"

Application::Application()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_CubeModel = nullptr;
    m_SphereModel = nullptr;
    m_GroundModel = nullptr;
    m_Light = nullptr;
    m_Light2 = nullptr;
    m_RenderTexture = nullptr;
    m_RenderTexture2 = nullptr;
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

    m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetLookAt(0.0f, 10.0f, 5.0f);
    m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

    m_RenderTexture = new RenderTexture;
    result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR, 1);
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

    m_Light2 = new Light;

    m_Light2->SetDiffuseColor(0.2f, 0.1f, 0.8f, 1.0f);
    m_Light2->SetLookAt(0.0f, 0.0f, 5.0f);
    m_Light2->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

    m_RenderTexture2 = new RenderTexture;
    result = m_RenderTexture2->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the second render to texture object.", L"Error", MB_OK);
        return false;
    }

    return true;
}

void Application::Shutdown()
{
    if (m_RenderTexture2)
    {
        m_RenderTexture2->Shutdown();
        delete m_RenderTexture2;
        m_RenderTexture2 = nullptr;
    }

    if (m_Light2)
    {
        delete m_Light2;
        m_Light2 = nullptr;
    }

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
    bool result;

    if (Input->IsEscapePressed())
    {
        return false;
    }

    m_Light->SetPosition(5.0f, 8.0f, -5.0f);
    m_Light->GenerateViewMatrix();

    m_Light2->SetPosition(-5.0f, 8.0f, -5.0f);
    m_Light2->GenerateViewMatrix();

    result = RenderDepthToTexture();
    if (!result)
    {
        return false;
    }

    result = RenderDepthToTexture2();
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

bool Application::RenderDepthToTexture()
{
    XMMATRIX translateMatrix, lightViewMatrix, lightProjectionMatrix;
    bool result;

    m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetProjectionMatrix(lightProjectionMatrix);

    translateMatrix = XMMatrixTranslation(-2.0f, 2.0f, 0.0f);

    m_CubeModel->Render(m_Direct3D->GetDeviceContext());

    result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), translateMatrix, lightViewMatrix, lightProjectionMatrix);
    if (!result)
    {
        return false;
    }

    translateMatrix = XMMatrixTranslation(2.0f, 2.0f, 0.0f);

    m_SphereModel->Render(m_Direct3D->GetDeviceContext());
    result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), translateMatrix, lightViewMatrix, lightProjectionMatrix);
    if (!result)
    {
        return false;
    }

    translateMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

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

bool Application::RenderDepthToTexture2()
{
    XMMATRIX translateMatrix, lightViewMatrix, lightProjectionMatrix;
    bool result;

    m_RenderTexture2->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_RenderTexture2->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_Light2->GetViewMatrix(lightViewMatrix);
    m_Light2->GetProjectionMatrix(lightProjectionMatrix);

    translateMatrix = XMMatrixTranslation(-2.0f, 2.0f, 0.0f);

    m_CubeModel->Render(m_Direct3D->GetDeviceContext());
    result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), translateMatrix, lightViewMatrix, lightProjectionMatrix);
    if (!result)
    {
        return false;
    }

    translateMatrix = XMMatrixTranslation(2.0f, 2.0f, 0.0f);

    m_SphereModel->Render(m_Direct3D->GetDeviceContext());
    result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), translateMatrix, lightViewMatrix, lightProjectionMatrix);
    if (!result)
    {
        return false;
    }

    translateMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

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
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, lightViewMatrix2, lightProjectionMatrix2;
    bool result;

    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetProjectionMatrix(lightProjectionMatrix);

    m_Light2->GetViewMatrix(lightViewMatrix2);
    m_Light2->GetProjectionMatrix(lightProjectionMatrix2);

    worldMatrix = XMMatrixTranslation(-2.0f, 2.0f, 0.0f);

    m_CubeModel->Render(m_Direct3D->GetDeviceContext());

    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_CubeModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(), m_shadowMapBias, lightViewMatrix2, lightProjectionMatrix2, m_RenderTexture2->GetShaderResourceView(), m_Light2->GetPosition(), m_Light2->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

    worldMatrix = XMMatrixTranslation(2.0f, 2.0f, 0.0f);

    m_SphereModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_SphereModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(), m_shadowMapBias, lightViewMatrix2, lightProjectionMatrix2, m_RenderTexture2->GetShaderResourceView(), m_Light2->GetPosition(), m_Light2->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

    worldMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_GroundModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(), m_shadowMapBias, lightViewMatrix2, lightProjectionMatrix2, m_RenderTexture2->GetShaderResourceView(), m_Light2->GetPosition(), m_Light2->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

    m_Direct3D->EndScene();

    return true;
}
