#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "timer.h"
#include "particlesystem.h"
#include "particleshader.h"

const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_NEAR = 0.3;
const float SCREEN_DEPTH = 1000.0f;

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
	bool Render();
	// bool RenderHeatToTexture();
	// bool RenderSceneToTexture(float);
	// bool RenderGBuffer();
	// bool RenderSSAO();
	// bool BlurSSAOTexture();
	// bool UpdateMouseStrings(int, int, bool);
	// bool TestIntersection(int, int);
	// bool RaySphereIntersect(XMFLOAT3, XMFLOAT3, float);

private:
	D3D* m_Direct3D;
	Timer* m_Timer;
	Camera* m_Camera;
	ParticleSystem* m_ParticleSystem; 
	ParticleShader* m_ParticleShader; 
};

#endif
