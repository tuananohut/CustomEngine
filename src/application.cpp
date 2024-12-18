#include "headers/application.h"

Application::Application()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_Model = nullptr;
    m_Light = nullptr;
    m_FullScreenWindow = nullptr;
    m_DeferredBuffers = nullptr;
    m_DeferredShader = nullptr;
    m_LightShader = nullptr;
}

Application::Application(const Application& other) {}

Application::~Application() {}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    char modelFilename[128];
    char textureFilename[128];
    bool result;

    m_Direct3D = new D3D;

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    m_Camera = new Camera;

    m_Camera->SetPosition(0.f, 0.f, -10.f);
    m_Camera->Render();
    m_Camera->RenderBaseViewMatrix();

    m_Light = new Light;

    m_Light->SetDiffuseColor(1.f, 1.f, 1.f, 1.f);
    m_Light->SetDirection(0.f, 0.f, 1.f);
        
    strcpy_s(modelFilename, "assets/models/cube.txt");
    strcpy_s(textureFilename, "assets/textures/stone01.tga");

    m_Model = new Model;
    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_FullScreenWindow = new OrthoWindow;
    m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the full screen window object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_DeferredBuffers = new DeferredBuffers;
    m_DeferredBuffers->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the deferred buffers object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_DeferredShader = new DeferredShader; 
    m_DeferredShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the deferred shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_LightShader = new LightShader;
    m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

void Application::Shutdown()
{
    if (m_FullScreenWindow)
    {
        m_FullScreenWindow->Shutdown();
        delete m_FullScreenWindow;
        m_FullScreenWindow = nullptr;
    }

    if (m_DeferredShader)
    {
        m_DeferredShader->Shutdown();
        delete m_DeferredShader;
        m_DeferredShader = nullptr;
    }

    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = nullptr;
    }

    if (m_Light)
    {
        delete m_Light;
        m_Light = nullptr;
    }

    if (m_DeferredBuffers)
    {
        m_DeferredBuffers->Shutdown();
        delete m_DeferredBuffers;
        m_DeferredBuffers = nullptr;
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
    char testString[32];
    int mouseX, mouseY;
    static float rotation = 0;
    bool result, mouseDown;
    static bool intersect;

    if (Input->IsEscapePressed() == true)
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

    m_DeferredBuffers->SetRenderTargets(m_Direct3D->GetDeviceContext());

    m_DeferredBuffers->ClearRenderTargets(m_Direct3D->GetDeviceContext(), 0.f, 0.f, 0.f, 1.f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    worldMatrix = XMMatrixRotationY(rotation);
    
    m_Model->Render(m_Direct3D->GetDeviceContext());
    result = m_DeferredShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
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
    bool result;

    m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetBaseViewMatrix(baseViewMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    m_Direct3D->TurnZBufferOff(); 

    m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

    m_Model->Render(m_Direct3D->GetDeviceContext());
    result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_DeferredBuffers->GetShaderResourceView(0), m_DeferredBuffers->GetShaderResourceView(1), m_Light->GetDirection());
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