#include "headers/application.h"

Application::Application()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_Model = nullptr;
    m_ColorShader = nullptr;
}

Application::Application(const Application& other) {}

Application::~Application() {}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    //char modelFilename[128];
    //char textureFilename[128];
    //char textureFilename1[128];
    //char textureFilename2[128];
    //char testString[32];
    //char mouseString1[32];
    //char mouseString2[32];
    //char mouseString3[32];
    bool result;

    m_Direct3D = new D3D;

    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    m_Camera = new Camera;

    m_Camera->SetPosition(0.f, 0.f, -5.f);
    m_Camera->Render();
    m_Camera->RenderBaseViewMatrix();
    

    // strcpy_s(modelFilename, "assets/models/sphere.txt");
    // strcpy_s(textureFilename, "assets/textures/red.tga");
    // strcpy_s(textureFilename1, "assets/textures/stone01.tga");
    // strcpy_s(textureFilename2, "assets/textures/glowmap001.tga");

    m_Model = new Model;
    result = m_Model->Initialize(m_Direct3D->GetDevice());
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_ColorShader = new ColorShader;
    result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    
    return true;
}

void Application::Shutdown()
{
    if (m_ColorShader)
    {
        m_ColorShader->Shutdown();
        delete m_ColorShader;
        m_ColorShader = nullptr;
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

    result = Render(rotation);
    if (!result)
    {
        return false;
    }

    return true;
}

bool Application::Render(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    float tessellationValue = 12.f; 
    bool result;

    m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.0f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_Model->Render(m_Direct3D->GetDeviceContext());
    result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, tessellationValue);
    if (!result)
    {
        return false;
    }

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
