#include "headers/application.h"
#include <cstring>
#include <random>

float generateRandomFloat(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

Application::Application()
{
    m_Direct3D = nullptr;
    m_Camera = nullptr;
    m_Timer = nullptr;
    m_ParticleShader = nullptr;
    m_ParticleSystem = nullptr;
    m_ParticleSystem1 = nullptr;
    m_ParticleSystem2 = nullptr;
    m_Planet = nullptr;
    m_TextureShader = nullptr;
    m_XAudio = nullptr;
    m_Sound = nullptr;
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
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    m_Camera = new Camera;

    m_Camera->SetPosition(0.0f, -1.0f, -10.0f);
    m_Camera->Render();

    m_Timer = new Timer;
    result = m_Timer->Initialize();
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    strcpy_s(textureFilename, "../CustomEngine/assets/textures/star01.tga");
    strcpy_s(textureFilename1, "../CustomEngine/assets/textures/star02.tga");
    strcpy_s(textureFilename2, "../CustomEngine/assets/textures/star03.tga");

    m_ParticleSystem = new ParticleSystem;
    result = m_ParticleSystem->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textures[0]);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the particle system object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_ParticleSystem1 = new ParticleSystem;
    result = m_ParticleSystem1->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textures[1]);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the particle system object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_ParticleSystem2 = new ParticleSystem;
    result = m_ParticleSystem2->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textures[2]);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the particle system object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_ParticleShader = new ParticleShader;
    result = m_ParticleShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the particle shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    strcpy_s(modelFilename, "../CustomEngine/assets/models/sphere.txt");
    strcpy_s(textureFilename1, "../CustomEngine/assets/textures/martian.tga");

    m_Planet = new Model;
    result = m_Planet->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename, textureFilename1, textureFilename2);
    if (!m_Planet)
    {
        MessageBox(hwnd, L"Could not initialize the planet object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_TextureShader = new TextureShader;
    result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!m_TextureShader)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_XAudio = new XAudio;
    result = m_XAudio->Initialize();
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the XAudio.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_Sound = new XAudioSound3D;
   
    strcpy_s(soundFilename, "../CustomEngine/assets/sounds/space.wav");

    result = m_Sound->LoadTrack(m_XAudio->GetXAudio2(), soundFilename, 1.f);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize test sound object 2.", L"Error", MB_OK);
        return false;
    }

    m_Sound->PlayTrack();

    return true;
}


void Application::Shutdown()
{
    if (m_ParticleShader)
    {
        m_ParticleShader->Shutdown();
        delete m_ParticleShader;
        m_ParticleShader = nullptr;
    }

    if (m_ParticleSystem)
    {
        m_ParticleSystem->Shutdown();
        delete m_ParticleSystem;
        m_ParticleSystem = nullptr;
    }

    if (m_ParticleSystem1)
    {
        m_ParticleSystem1->Shutdown();
        delete m_ParticleSystem1;
        m_ParticleSystem1 = nullptr;
    }

    if (m_ParticleSystem2)
    {
        m_ParticleSystem2->Shutdown();
        delete m_ParticleSystem2;
        m_ParticleSystem2 = nullptr;
    }

    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = nullptr;
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

    if (m_Timer)
    {
        delete m_Timer;
        m_Timer = nullptr;
    }

    if (m_Planet)
    {
        m_Planet->Shutdown();
        delete m_Planet;
        m_Planet = nullptr;
    }
}


bool Application::Frame(Input* Input)
{
    static float rotation = 0.0f;
    bool result;
    static float x = 10;
    static float y = 10;
    static float z = 10;

    m_Timer->Frame();

    if (Input->IsEscapePressed())
    {
        return false;
    }

    result = SoundProcessing();
    if (!result)
    {
        return false;
    }

    result = m_ParticleSystem->Frame(m_Timer->GetTime(), m_Direct3D->GetDeviceContext());
    if (!result)
    {
        return false;
    }


    result = m_ParticleSystem1->Frame(m_Timer->GetTime(), m_Direct3D->GetDeviceContext());
    if (!result)
    {
        return false;
    }


    result = m_ParticleSystem2->Frame(m_Timer->GetTime(), m_Direct3D->GetDeviceContext());
    if (!result)
    {
        return false;
    }

    rotation -= 0.0174532925f * 0.25f;
    if (rotation < 0.0f)
    {
        rotation += 360.0f;
    }

    if (y <= -15.f)
    {
        x = generateRandomFloat(-3.f, 3.f);
        y = generateRandomFloat(10.f, 20.f);
        z = 15.f;
    }

    else
    {
        x += 0.1f * m_Timer->GetTime();
        y -= 3.f * m_Timer->GetTime();
        z -= 1.f * m_Timer->GetTime();
    }

    result = Render(rotation, x, y, z);
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

bool Application::Render(float rotation, float x, float y, float z)
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    XMMATRIX move;
    bool result;

    m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);

    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    m_Direct3D->EnableAlphaBlending();

    worldMatrix = XMMatrixRotationY(rotation);

    move = XMMatrixTranslation(x, y, z);
    worldMatrix *= move;

    m_Planet->Render(m_Direct3D->GetDeviceContext());

    result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Planet->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Planet->GetTexture(1));
    if (!result)
    {
        return false;
    }

    m_Direct3D->GetWorldMatrix(worldMatrix);

    m_Direct3D->TurnZBufferOff();

    m_ParticleSystem->Render(m_Direct3D->GetDeviceContext());
    result = m_ParticleShader->Render(m_Direct3D->GetDeviceContext(), m_ParticleSystem->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ParticleSystem->GetTexture());
    if (!result)
    {
        return false;
    }

    m_ParticleSystem1->Render(m_Direct3D->GetDeviceContext());
    result = m_ParticleShader->Render(m_Direct3D->GetDeviceContext(), m_ParticleSystem1->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ParticleSystem1->GetTexture());
    if (!result)
    {
        return false;
    }

    m_ParticleSystem2->Render(m_Direct3D->GetDeviceContext());

    result = m_ParticleShader->Render(m_Direct3D->GetDeviceContext(), m_ParticleSystem2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_ParticleSystem2->GetTexture());
    if (!result)
    {
        return false;
    }

    m_Direct3D->DisableAlphaBlending();
    m_Direct3D->TurnZBufferOn();
    
    m_Direct3D->EndScene();

    return true;
}
