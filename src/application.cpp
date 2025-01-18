#include "headers/application.h"

Application::Application()
{
    m_Direct3D = nullptr;
    m_Timer = nullptr; 
    m_Camera = nullptr;
    m_Model = nullptr;
    m_Light = nullptr;
    m_LightShader = nullptr;
    m_RenderTexture = nullptr; 
    m_FullScreenWindow = nullptr;
    m_TextureShader = nullptr; 
    m_BlurShader = nullptr;
    m_Blur = nullptr; 
    m_Heat = nullptr; 
    m_HeatShader = nullptr;
    m_HeatTexture = nullptr;
}

Application::Application(const Application& other) {}

Application::~Application() {}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    char modelFilename[128];
    char diffuseFilename[128];
    char normalFilename[128];
    char rmFilename[128];
    int downSampleWidth, downSampleHeight; 
    bool result;

    m_Direct3D = new D3D;
    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_Timer = new Timer; 
    result = m_Timer->Initialize();
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_Camera = new Camera;

    m_Camera->SetPosition(0.f, 0.f, -5.f);
    m_Camera->Render();
    m_Camera->RenderBaseViewMatrix();

    strcpy_s(modelFilename, "assets/models/sphere.txt");
    strcpy_s(diffuseFilename, "assets/textures/yellowcolor01.tga");
    strcpy_s(normalFilename, "assets/textures/pirate_normal.tga");
    strcpy_s(rmFilename, "assets/textures/pirate_roughness.tga");

    m_Model = new Model;
    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, diffuseFilename, normalFilename, rmFilename);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_Light = new Light; 

    m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.f);
    m_Light->SetDiffuseColor(1.f, 1.f, 1.f, 1.f);
    m_Light->SetDirection(0.f, 0.f, 1.f);

    m_LightShader = new LightShader; 
    result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd); 
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false; 
    }
    
    m_RenderTexture = new RenderTexture;
    result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        MessageBox(hwnd, L"Could no initialize the render texture object.", L"Error", MB_OK | MB_ICONERROR); 
        return false; 
    }

    m_FullScreenWindow = new OrthoWindow; 
    result = m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight); 
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK | MB_ICONERROR); 
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

    downSampleWidth = screenWidth / 2;
    downSampleHeight = screenHeight / 2;

    m_Blur = new Blur;
    result = m_Blur->Initialize(m_Direct3D, downSampleWidth, downSampleHeight, SCREEN_NEAR, SCREEN_DEPTH, screenWidth, screenHeight); 
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the blur object.", L"Error", MB_OK | MB_ICONERROR); 
        return false;
    }

    m_Heat = new Heat; 
    result = m_Heat->Initialize(m_Direct3D);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the heat object.", L"Error", MB_OK | MB_ICONERROR); 
        return false; 
    }

    m_HeatShader = new HeatShader; 
    result = m_HeatShader->Initialize(m_Direct3D->GetDevice(), hwnd); 
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the heat shader object.", L"Error", MB_OK | MB_ICONERROR); 
        return false; 
    }

    m_HeatTexture = new RenderTexture; 
    result = m_HeatTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, 1);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the heat render texture object.", L"Error", MB_OK | MB_ICONERROR); 
        return false; 
    }

    return true;
}

void Application::Shutdown()
{
    if (m_HeatTexture)
    {
        m_HeatTexture->Shutdown();
        delete m_HeatTexture; 
        m_HeatTexture = nullptr; 
    }
    
    if (m_HeatShader)
    {
        m_HeatShader->Shutdown();
        delete m_HeatShader;
        m_HeatShader = nullptr; 
    }

    if (m_Heat)
    {
        m_Heat->Shutdown();
        delete m_Heat; 
        m_Heat = nullptr; 
    }

    if (m_Blur)
    {
        m_Blur->Shutdown();
        delete m_Blur; 
        m_Blur = nullptr; 
    }

    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = nullptr; 
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

    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = nullptr; 
    }

    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = nullptr;
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

    if (m_Timer)
    {
        delete m_Timer;
        m_Timer = nullptr; 
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
    float frameTime; 
    static float rotation = 0.f;

    if (Input->IsEscapePressed() == true)
    {
        return false;
    }

    m_Timer->Frame();
    frameTime = m_Timer->GetTime();

    m_Heat->Frame(frameTime); 

    result = RenderSceneToTexture(rotation); 
    if (!result)
    {
        return false;
    }

    result = RenderHeatToTexture();
    if (!result)
    {
        return false; 
    }

    result = m_Blur->BlurTexture(m_Direct3D, m_Camera, m_HeatTexture, m_TextureShader, m_BlurShader);
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

bool Application::RenderSceneToTexture(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.25f, 0.25f, 0.25f, 1.f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_Model->Render(m_Direct3D->GetDeviceContext());
    result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(0), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

    m_Direct3D->SetBackBufferRenderTarget();
    m_Direct3D->ResetViewport();

    return true;
}

bool Application::RenderHeatToTexture()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    m_HeatTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
    m_HeatTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_Model->Render(m_Direct3D->GetDeviceContext());
    result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(0), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
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
    XMFLOAT3 scrollSpeeds, scales;
    XMFLOAT2 distortion1, distortion2, distortion3;
    float distortionScale, distortionBias, emessiveMultiplier, noiseFrameTime;
    bool result;

    m_Heat->GetNoiseValues(scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias, emessiveMultiplier, noiseFrameTime);

    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetBaseViewMatrix(baseViewMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    m_Direct3D->TurnZBufferOff();

    m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

        result = m_HeatShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView(),
            m_HeatTexture->GetShaderResourceView(), emessiveMultiplier, noiseFrameTime, scrollSpeeds, scales, distortion1, distortion2, distortion3,
            distortionScale, distortionBias, m_Heat->GetTexture());
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
bool Application::RenderGBuffer()
{
    XMMATRIX translateMatrix, viewMatrix, projectionMatrix;
    bool result;

    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_DeferredBuffers->SetRenderTargets(m_Direct3D->GetDeviceContext());
    m_DeferredBuffers->ClearRenderTargets(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 0.f);

    translateMatrix = XMMatrixMultiply(XMMatrixTranslation(2.f, 1.f, 0.f), XMMatrixRotationY(90.f));
    translateMatrix = XMMatrixMultiply(XMMatrixScaling(2.f, 2.f, 2.f), translateMatrix);
    // ranslateMatrix = ;

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
*/