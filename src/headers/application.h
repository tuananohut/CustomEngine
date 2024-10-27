#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "timer.h"
#include "particlesystem.h"
#include "particleshader.h"
#include "model.h"
#include "textureshader.h"

const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class Application
{
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(Input*);

private:
	// bool RenderSceneToTexture(float, float, float, bool);
	// bool SoundProcessing();
	bool Render(float);

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Timer* m_Timer;
	ParticleSystem* m_ParticleSystem;
	ParticleShader* m_ParticleShader;
	Model* m_Planet;
	TextureShader* m_TextureShader;
};

#endif
