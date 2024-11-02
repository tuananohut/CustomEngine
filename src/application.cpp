#include "headers/application.h"

Application::Application()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_GroundModel = nullptr;
    m_CubeModel = nullptr;
    m_ProjectionShader = nullptr;
    m_ProjectionTexture = nullptr;
    m_ViewPoint = nullptr;
    m_Light = nullptr;
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
    
    char* textures[3] = {textureFilename, textureFilename1, textureFilename2};
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
    strcpy_s(textureFilename, "../CustomEngine/assets/textures/palestine.tga");

    m_CubeModel = new Model;
    result = m_CubeModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    
    m_ProjectionShader = new ProjectionShader;
    result = m_ProjectionShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the projection shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    strcpy_s(textureFilename, "../CustomEngine/assets/textures/grate.tga");

    m_ProjectionTexture = new Texture;
    result = m_ProjectionTexture->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the projection texture object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_ViewPoint = new ViewPoint;

    m_ViewPoint->SetPosition(2.0f, 5.0f, -2.0f);
    m_ViewPoint->SetLookAt(0.f, 0.f, 0.f);
    m_ViewPoint->SetProjectionParameters((3.14159265358979323846f / 2.0f), 1.0f, 0.1f, 100.0f);
    m_ViewPoint->GenerateViewMatrix();
    m_ViewPoint->GenerateProjectionMatrix();

    m_Light = new Light;

    m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.f);
    m_Light->SetDiffuseColor(1.f, 1.f, 1.f, 1.f);
    m_Light->SetPosition(2.f, 5.f, -2.f);

    return true;
}


void Application::Shutdown()
{
    if (m_Light)
    {
        delete m_Light;
        m_Light = nullptr;
    }

    if (m_ViewPoint)
    {
        delete m_ViewPoint;
        m_ViewPoint = nullptr;
    }

    if (m_ProjectionTexture)
    {
        m_ProjectionTexture->Shutdown();
        delete m_ProjectionTexture;
        m_ProjectionTexture = nullptr;
    }

    if (m_ProjectionShader)
    {
        m_ProjectionShader->Shutdown();
        delete m_ProjectionShader;
        m_ProjectionShader = nullptr;
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
    bool result;

    if (Input->IsEscapePressed())
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

bool Application::Render(float rotation)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, viewMatrix2, projectionMatrix2;
    bool result;
    float brightness;

    m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_ViewPoint->GetViewMatrix(viewMatrix2);
    m_ViewPoint->GetProjectionMatrix(projectionMatrix2);

    brightness = 1.5f;

    worldMatrix = XMMatrixTranslation(0.f, 1.f, 0.f);

    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ProjectionShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, viewMatrix2, projectionMatrix2, m_GroundModel->GetTexture(0), m_ProjectionTexture->GetTexture(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(), brightness);
    if (!result)
    {
        return false;
    }
    
    worldMatrix = XMMatrixMultiply(XMMatrixTranslation(0.f, 2.f, 0.f), XMMatrixRotationY(rotation));

    m_CubeModel->Render(m_Direct3D->GetDeviceContext());
    result = m_ProjectionShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, viewMatrix2, projectionMatrix2, m_CubeModel->GetTexture(0), m_ProjectionTexture->GetTexture(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(), brightness);
    if (!result)
    {
        return false;
    }

    m_Direct3D->EndScene();

    return true;
}
