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
    m_BlackWhiteRenderTexture = nullptr;
    m_DepthShader = nullptr;
    m_ShadowShader = nullptr;
    m_SoftShadowShader = nullptr;
    m_Blur = nullptr;
    m_TextureShader = nullptr;
    m_BlurShader = nullptr;
}

Application::Application(const Application& other) {}

Application::~Application() {}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    char modelFilename[128];
    char textureFilename[128];
    char textureFilename1[128];
    char textureFilename2[128];
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

    m_Camera->SetPosition(0.f, 0.f, -10.0f);
    m_Camera->Render();
    m_Camera->RenderBaseViewMatrix();

    m_Camera->SetPosition(0.f, 7.f, -10.0f);
    m_Camera->SetRotation(35.0f, 0.f, 0.f);
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
    m_Light->SetLookAt(0.f, 0.f, 0.f);
    m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

    m_RenderTexture = new RenderTexture;
    result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the render texture object.", L"Error", MB_OK);
        return false;
    }

    m_BlackWhiteRenderTexture = new RenderTexture;
    result = m_BlackWhiteRenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the black an white render texture object.", L"Error", MB_OK | MB_ICONERROR);
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

    m_SoftShadowShader = new SoftShadowShader;
    result = m_SoftShadowShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the soft shadow shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    downSampleWidth = SHADOWMAP_WIDTH / 2;
    downSampleHeight = SHADOWMAP_HEIGHT / 2;

    m_Blur = new Blur;
    result = m_Blur->Initialize(m_Direct3D, downSampleWidth, downSampleHeight, SCREEN_NEAR, SCREEN_DEPTH, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the blur object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_TextureShader = new TextureShader;
    result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_BlurShader = new BlurShader;
    result = m_BlurShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the blur shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_shadowMapBias = 0.0022f;
    
    return true;
}

void Application::Shutdown()
{
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

    if (m_Blur)
    {
        m_Blur->Shutdown();
        delete m_Blur;
        m_Blur = nullptr;
    }

    if (m_SoftShadowShader)
    {
        m_SoftShadowShader->Shutdown();
        delete m_SoftShadowShader;
        m_SoftShadowShader = nullptr;
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

    if (m_BlackWhiteRenderTexture)
    {
        m_BlackWhiteRenderTexture->Shutdown();
        delete m_BlackWhiteRenderTexture;
        m_BlackWhiteRenderTexture = nullptr;
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
    static float lightPosX = -5.f;
    bool result;
    XMMATRIX translateMatrix = {}, lightViewMatrix = {}, lightOrthoMatrix = {};

    if (Input->IsEscapePressed())
    {
        return false;
    }

    lightPosX += 0.05f;
    if (lightPosX > 5.f)
    {
        lightPosX = -5.f;
    } 

    m_Light->SetPosition(lightPosX, 8.0f, -5.f);
    m_Light->GenerateViewMatrix();

    result = RenderDepthToTexture(translateMatrix, lightViewMatrix, lightOrthoMatrix, m_RenderTexture, m_Light);
    if (!result)
    {
        return false;
    }

    result = RenderBlackAndWhiteShadows();
    if (!result)
    {
        return false;
    }

    result = m_Blur->BlurTexture(m_Direct3D, m_Camera, m_BlackWhiteRenderTexture, m_TextureShader, m_BlurShader);
    if (!result)
    {
        return true;
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

bool Application::RenderBlackAndWhiteShadows()
{
    XMMATRIX translateMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix;
    bool result;

    m_BlackWhiteRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_BlackWhiteRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetProjectionMatrix(lightProjectionMatrix);

    translateMatrix = XMMatrixTranslation(-2.f, 2.f, 0.f);

    m_CubeModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), translateMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    translateMatrix = XMMatrixTranslation(2.f, 2.f, 0.f);
    
    m_SphereModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), translateMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    translateMatrix = XMMatrixTranslation(0.f, 1.f, 0.f);

    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), translateMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(), m_shadowMapBias);
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

    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetProjectionMatrix(lightProjectionMatrix);

    worldMatrix = XMMatrixTranslation(-2.0f, 2.0f, 0.0f);

    m_CubeModel->Render(m_Direct3D->GetDeviceContext());
    result = m_SoftShadowShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_CubeModel->GetTexture(0), m_BlackWhiteRenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    worldMatrix = XMMatrixTranslation(2.0f, 2.0f, 0.0f);

    m_SphereModel->Render(m_Direct3D->GetDeviceContext());
    result = m_SoftShadowShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_SphereModel->GetTexture(0), m_BlackWhiteRenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    worldMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    result = m_SoftShadowShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_GroundModel->GetTexture(0), m_BlackWhiteRenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    m_Direct3D->EndScene();

    return true;
}
