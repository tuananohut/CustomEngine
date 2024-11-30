#include "headers/application.h"

Application::Application()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_TreeTrunkModel = nullptr;
    m_GroundModel = nullptr;
    m_TreeLeafModel = nullptr;
    m_Light = nullptr;
    m_RenderTexture = nullptr;
    m_DepthShader = nullptr;
    m_TransparentDepthShader = nullptr;
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

    m_Camera->SetPosition(0.f, 7.f, -11.0f);
    m_Camera->SetRotation(20.0f, 0.f, 0.f);
    m_Camera->Render();

    m_TreeTrunkModel = new Model;

    strcpy_s(modelFilename, "assets/models/trunk001.txt");
    strcpy_s(textureFilename, "assets/textures/trunk001.tga");
    strcpy_s(textureFilename1, "assets/textures/stone01.tga");
    strcpy_s(textureFilename2, "assets/textures/stone01.tga");

    result = m_TreeTrunkModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
        return false;
    }

    m_TreeLeafModel = new Model;

    strcpy_s(modelFilename, "assets/models/leaf001.txt");
    strcpy_s(textureFilename, "assets/textures/leaf001.tga");

    result = m_TreeLeafModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK);
        return false;
    }

    m_GroundModel = new Model;

    strcpy_s(modelFilename, "assets/models/plane01.txt");
    strcpy_s(textureFilename, "assets/textures/dirt.tga");

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

    m_TransparentDepthShader = new TransparentDepthShader;
    result = m_TransparentDepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the transparent depth shader object.", L"Error", MB_OK);
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

    if (m_TransparentDepthShader)
    {
        m_TransparentDepthShader->Shutdown();
        delete m_TransparentDepthShader;
        m_TransparentDepthShader = nullptr;
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

    if (m_TreeLeafModel)
    {
        m_TreeLeafModel->Shutdown();
        delete m_TreeLeafModel;
        m_TreeLeafModel = nullptr;
    }

    if (m_TreeTrunkModel)
    {
        m_TreeTrunkModel->Shutdown();
        delete m_TreeTrunkModel;
        m_TreeTrunkModel = nullptr;
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
    static float lightPosX = 9.f;
    float frameTime;
    float radians;
    bool result;
    XMMATRIX translateMatrix = {}, lightViewMatrix = {}, lightProjectionMatrix = {};

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

    m_Light->SetPosition(lightPosX, 10.f, 1.f);
    m_Light->SetLookAt(-lightPosX, 0.f, 2.f);
    m_Light->GenerateViewMatrix();

    result = RenderSceneToTexture();
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

/*
bool Application::RenderDepthToTexture(XMMATRIX translateMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, RenderTexture* m_RenderTexture, Light* m_Light)
{
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
*/

bool Application::RenderSceneToTexture()
{
    XMMATRIX worldMatrix, scaleMatrix, translateMatrix, lightViewMatrix, lightOrthoMatrix;
    bool result;

    m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
    translateMatrix = XMMatrixTranslation(0.f, 1.f, 0.f);
    worldMatrix = XMMatrixMultiply(scaleMatrix, translateMatrix);

    m_TreeTrunkModel->Render(m_Direct3D->GetDeviceContext());
    result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_TreeTrunkModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);
    if (!result)
    {
        return false;
    }

    m_TreeLeafModel->Render(m_Direct3D->GetDeviceContext());
    result = m_TransparentDepthShader->Render(m_Direct3D->GetDeviceContext(), m_TreeLeafModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix, m_TreeLeafModel->GetTexture(0));
    if (!result)
    {
        return false;
    }

    scaleMatrix = XMMatrixScaling(2.f, 2.f, 2.f);
    translateMatrix = XMMatrixTranslation(0.f, 1.f, 0.f);
    worldMatrix = XMMatrixMultiply(scaleMatrix, translateMatrix);

    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);
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
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightOrthoMatrix, scaleMatrix, translateMatrix;
    bool result;

    m_Direct3D->BeginScene(0.0f, 0.5f, 0.8f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    scaleMatrix = XMMatrixScaling(2.0f, 2.0f, 2.0f);
    translateMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
    worldMatrix = XMMatrixMultiply(scaleMatrix, translateMatrix);

    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_GroundModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
    translateMatrix = XMMatrixTranslation(0.f, 1.f, 0.f);
    worldMatrix = XMMatrixMultiply(scaleMatrix, translateMatrix);

    m_TreeTrunkModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_TreeTrunkModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_TreeTrunkModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }
    
    m_Direct3D->EnableAlphaBlending();

    m_TreeLeafModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_TreeLeafModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_TreeLeafModel->GetTexture(0), m_RenderTexture->GetShaderResourceView(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_shadowMapBias);
    if (!result)
    {
        return false;
    }

    m_Direct3D->DisableAlphaBlending();

    m_Direct3D->EndScene();

    return true;
}
