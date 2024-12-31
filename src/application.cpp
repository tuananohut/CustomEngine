#include "headers/application.h"

Application::Application()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_Light = nullptr;
    m_SphereModel = nullptr;
    m_GroundModel = nullptr;
    m_DeferredBuffers = nullptr;
    m_GBufferShader = nullptr;
    m_SSAORenderTexture = nullptr;
    m_FullScreenWindow = nullptr;
    m_SSAOShader = nullptr;
    m_RandomTexture = nullptr;
    m_BlurSSAORenderTexture = nullptr;
    m_SSAOBlurShader = nullptr;
    m_LightShader = nullptr;
}

Application::Application(const Application& other) {}

Application::~Application() {}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    char modelFilename[128];
    char textureFilename[128];
    char textureFilename2[128];
    bool result;

    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    m_Direct3D = new D3D;
    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_Camera = new Camera;

    m_Camera->SetPosition(0.f, 0.f, -10.f);
    m_Camera->RenderBaseViewMatrix();

    m_Camera->SetPosition(0.f, 7.f, -10.f);
    m_Camera->SetRotation(35.f, 0.f, 0.f);
    m_Camera->Render();

    m_Light = new Light;

    m_Light->SetDiffuseColor(1.f, 1.f, 1.f, 1.f);
    m_Light->SetDirection(1.f, -0.5f, 0.f);

    strcpy_s(modelFilename, "assets/models/sphere.txt");
    strcpy_s(textureFilename, "assets/textures/ice01.tga");
    strcpy_s(textureFilename2, "assets/textures/metal001.tga");

    m_SphereModel = new Model;
    result = m_SphereModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    strcpy_s(modelFilename, "assets/models/plane01.txt");

    m_GroundModel = new Model;
    result = m_GroundModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_DeferredBuffers = new DeferredBuffers;
    result = m_DeferredBuffers->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the deferred buffers object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_GBufferShader = new GBufferShader;
    result = m_GBufferShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the gbuffer shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_SSAORenderTexture = new RenderTexture;
    result = m_SSAORenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, 2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the ssao render texture object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_BlurSSAORenderTexture = new RenderTexture;
    result = m_BlurSSAORenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, 2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the blur ssao render texture object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_FullScreenWindow = new OrthoWindow;
    result = m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_SSAOShader = new SSAOShader;
    result = m_SSAOShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the ssao shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    strcpy_s(textureFilename, "assets/textures/random_vec.tga");
    
    m_RandomTexture = new Texture;
    result = m_RandomTexture->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the random texture object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_SSAOBlurShader = new SSAOBlurShader;
    result = m_SSAOBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the ssao blur shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_LightShader = new LightShader;
    result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

void Application::Shutdown()
{
    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = nullptr;
    }

    if (m_SSAOBlurShader)
    {
        m_SSAOBlurShader->Shutdown();
        delete m_SSAOBlurShader;
        m_SSAOBlurShader = nullptr;
    }

    if (m_RandomTexture)
    {
        m_RandomTexture->Shutdown();
        delete m_RandomTexture;
        m_RandomTexture = nullptr;
    }

    if (m_SSAOShader)
    {
        m_SSAOShader->Shutdown();
        delete m_SSAOShader;
        m_SSAOShader = nullptr;
    }

    if (m_FullScreenWindow)
    {
        m_FullScreenWindow->Shutdown();
        delete m_FullScreenWindow;
        m_FullScreenWindow = nullptr;
    }

    if (m_BlurSSAORenderTexture)
    {
        m_BlurSSAORenderTexture->Shutdown();
        delete m_BlurSSAORenderTexture;
        m_BlurSSAORenderTexture = nullptr;
    }

    if (m_SSAORenderTexture)
    {
        m_SSAORenderTexture->Shutdown();
        delete m_SSAORenderTexture;
        m_SSAORenderTexture = nullptr;
    }

    if (m_GBufferShader)
    {
        m_GBufferShader->Shutdown();
        delete m_GBufferShader;
        m_GBufferShader = nullptr;
    }

    if (m_DeferredBuffers)
    {
        m_DeferredBuffers->Shutdown();
        delete m_DeferredBuffers;
        m_DeferredBuffers = nullptr;
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

    if (m_Light)
    {
        delete m_Light;
        m_Light = nullptr;
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
    bool result;
    float rotation = 0.f;

    if (Input->IsEscapePressed() == true)
    {
        return false;
    }

    result = RenderGBuffer();
    if (!result)
    {
        return false;
    }

    result = RenderSSAO();
    if (!result)
    {
        return false;
    }

    result = BlurSSAOTexture();
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

bool Application::RenderGBuffer()
{
    XMMATRIX translateMatrix, viewMatrix, projectionMatrix;
    bool result;

    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_DeferredBuffers->SetRenderTargets(m_Direct3D->GetDeviceContext());
    m_DeferredBuffers->ClearRenderTargets(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 0.f);

    translateMatrix = XMMatrixTranslation(2.f, 2.f, 0.f);

    m_SphereModel->Render(m_Direct3D->GetDeviceContext());
    result = m_GBufferShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), translateMatrix, viewMatrix, projectionMatrix, m_SphereModel->GetTexture(0));
    if (!result)
    {
        return false;
    }

    translateMatrix = XMMatrixTranslation(0.f, 1.f, 0.f);

    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    result = m_GBufferShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), translateMatrix, viewMatrix, projectionMatrix, m_GroundModel->GetTexture(1));
    if (!result)
    {
        return false;
    }

    m_Direct3D->SetBackBufferRenderTarget();
    m_Direct3D->ResetViewport();

    return true;
}

bool Application::RenderSSAO()
{
    XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
    float sampleRadius, ssaoScale, ssaoBias, ssaoIntensity, randomTextureSize, screenWidth, screenHeight;
    bool result;

    sampleRadius = 1.f;
    ssaoScale = 1.f;
    ssaoBias = 0.1f;
    ssaoIntensity = 2.f;
    randomTextureSize = 64.f;

    screenWidth = (float)m_screenWidth;
    screenHeight = (float)m_screenHeight;

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetBaseViewMatrix(baseViewMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    m_SSAORenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_SSAORenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 0.f);

    m_Direct3D->TurnZBufferOff();

    m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());
    result = m_SSAOShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_DeferredBuffers->GetShaderResourcePositions(),m_DeferredBuffers->GetShaderResourceNormals(), m_RandomTexture->GetTexture(), screenWidth, screenHeight, randomTextureSize, sampleRadius, ssaoScale, ssaoBias, ssaoIntensity);
    if (!result)
    {
        return false;
    }

    m_Direct3D->TurnZBufferOn();

    m_Direct3D->SetBackBufferRenderTarget();
    m_Direct3D->ResetViewport();

    return true;
}


bool Application::BlurSSAOTexture()
{
    XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
    bool result;

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetBaseViewMatrix(baseViewMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    m_Direct3D->TurnZBufferOff();

    m_BlurSSAORenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_BlurSSAORenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

    m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());
    result = m_SSAOBlurShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_SSAORenderTexture->GetShaderResourceView(), m_DeferredBuffers->GetShaderResourceNormals(), m_screenWidth, m_screenHeight, 0);
    if (!result)
    {
        return false;
    }

    m_SSAORenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_SSAORenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

    m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());
    result = m_SSAOBlurShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_BlurSSAORenderTexture->GetShaderResourceView(), m_DeferredBuffers->GetShaderResourceNormals(), m_screenWidth, m_screenHeight, 1);
    if (!result)
    {
        return false;
    }

    m_Direct3D->TurnZBufferOn();

    m_Direct3D->SetBackBufferRenderTarget();
    m_Direct3D->ResetViewport();

    return true;
}

bool Application::Render(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, baseViewMatrix, orthoMatrix;
    bool result;

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Camera->GetBaseViewMatrix(baseViewMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);

    m_Direct3D->TurnZBufferOff();

    m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());
    result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Camera->GetPosition(), m_DeferredBuffers->GetShaderResourceNormals(), m_SSAORenderTexture->GetShaderResourceView(), viewMatrix, m_DeferredBuffers->GetShaderResourceColors());
    if (!result)
    {
        return false;
    }

    m_Direct3D->TurnZBufferOn();

    m_Direct3D->EndScene();

    return true;
}

/*
bool Application::TestIntersection(int mouseX, int mouseY)
{
    XMMATRIX projectionMatrix, viewMatrix, inverseViewMatrix, worldMatrix, inverseWorldMatrix;
    XMFLOAT4X4 pMatrix, iViewMatrix;
    XMVECTOR direction, origin, rayOrigin, rayDirection;
    XMFLOAT3 cameraDirection, cameraOrigin, rayOri, rayDir;
    float pointX, pointY;
    bool intersect;

    pointX = ((2.f * (float)mouseX) / (float)m_screenWidth) - 1.f;
    pointY = (((2.f * (float)mouseY) / (float)m_screenHeight) - 1.f) * -1.f;

    m_Direct3D->GetProjectionMatrix(projectionMatrix);
    XMStoreFloat4x4(&pMatrix, projectionMatrix);
    pointX = pointX / pMatrix._11;
    pointY = pointY / pMatrix._22;

    m_Camera->GetViewMatrix(viewMatrix);
    inverseViewMatrix = XMMatrixInverse(NULL, viewMatrix);
    XMStoreFloat4x4(&iViewMatrix, inverseViewMatrix);

    cameraDirection.x = (pointX * iViewMatrix._11) + (pointY * iViewMatrix._21) + iViewMatrix._31;
    cameraDirection.y = (pointX * iViewMatrix._12) + (pointY * iViewMatrix._22) + iViewMatrix._32;
    cameraDirection.z = (pointX * iViewMatrix._13) + (pointY * iViewMatrix._23) + iViewMatrix._33;
    direction = XMLoadFloat3(&cameraDirection);

    cameraOrigin = m_Camera->GetPosition();
    origin = XMLoadFloat3(&cameraOrigin);

    worldMatrix = XMMatrixTranslation(-5.f, 1.f, 5.f);

    inverseWorldMatrix = XMMatrixInverse(NULL, worldMatrix);

    rayOrigin = XMVector3TransformCoord(origin, inverseWorldMatrix);
    rayDirection = XMVector3TransformNormal(direction, inverseWorldMatrix);

    rayDirection = XMVector3Normalize(rayDirection);

    XMStoreFloat3(&rayOri, rayOrigin);
    XMStoreFloat3(&rayDir, rayDirection);

    intersect = RaySphereIntersect(rayOri, rayDir, 1.f);

    return intersect;
}

bool Application::RaySphereIntersect(XMFLOAT3 rayOrigin, XMFLOAT3 rayDirection, float radius)
{
    float a, b, c, discriminant;

    a = (rayDirection.x * rayDirection.x) + (rayDirection.y * rayDirection.y) + (rayDirection.z * rayDirection.z);
    b = ((rayDirection.x * rayOrigin.x) + (rayDirection.y * rayOrigin.y) + (rayDirection.z * rayOrigin.z)) * 2.0f;
    c = ((rayOrigin.x * rayOrigin.x) + (rayOrigin.y * rayOrigin.y) + (rayOrigin.z * rayOrigin.z)) - (radius * radius);

    discriminant = (b * b) - (4 * a * c);

    if (discriminant < 0.0f)
    {
        return false;
    }

    return true;
}

bool Application::UpdateMouseStrings(int mouseX, int mouseY, bool mouseDown)
{
    char tempString[16], finalString[32];
    bool result;

    sprintf_s(tempString, "%d", mouseX);

    strcpy_s(finalString, "Mouse X: ");
    strcat_s(finalString, tempString);

    result = m_MouseStrings[0].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 500, 10, 1.0f, 1.0f, 1.0f);
    if (!result)
    {
        return false;
    }

    sprintf_s(tempString, "%d", mouseY);

    strcpy_s(finalString, "Mouse Y: ");
    strcat_s(finalString, tempString);

    result = m_MouseStrings[1].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 500, 35, 1.0f, 1.0f, 1.0f);
    if (!result)
    {
        return false;
    }

    if (mouseDown)
    {
        strcpy_s(finalString, "Mouse Button: Yes");
    }
    else
    {
        strcpy_s(finalString, "Mouse Button: No");
    }

    result = m_MouseStrings[2].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 500, 60, 1.0f, 1.0f, 1.0f);
    if (!result)
    {
        return false;
    }

    return true;
}
*/

/*
bool Application::RenderSceneToTexture(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    m_DeferredBuffers->SetRenderTargets(m_Direct3D->GetDeviceContext());

    m_DeferredBuffers->ClearRenderTargets(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    worldMatrix = XMMatrixMultiply(XMMatrixRotationY(rotation), XMMatrixRotationX(rotation));
    // worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(2.f, 2.f, 2.f));
    //  = XMMatrixRotationY(rotation);
   //  worldMatrix = XMMatrixRotationY(rotation);

    m_Model->Render(m_Direct3D->GetDeviceContext());
    result = m_DeferredShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(0), m_Model->GetTexture(1));
    if (!result)
    {
        return false;
    }

    m_Direct3D->SetBackBufferRenderTarget();
    m_Direct3D->ResetViewport();

    return true;
}
*/