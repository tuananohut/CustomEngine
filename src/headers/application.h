#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <Windows.h>
#include <cmath>

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "textureshader.h"
#include "rendertexture.h"
#include "orthowindow.h"
#include "fadeshader.h"
#include "timer.h"

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
	bool RenderSceneToTexture(float, float, float, bool);
	// bool SoundProcessing();
	bool Render(float);

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_Cube;
	Model* m_IcoSphere;
	TextureShader* m_TextureShader;
	RenderTexture* m_RenderTexture;
	RenderTexture* m_RenderTexture2;
	OrthoWindow* m_FullScreenWindow;
	FadeShader* m_FadeShader;
	Timer* m_Timer;
	float m_accumulatedTime, m_fadeInTime;

	int scenes[2] = { 0, 1 };
	static int scene;

	float fadeTimer = 0.f;
	bool isSceneChanging = false;
	float fadeDuration = 1.f;
	int previousScene = scenes[scene];
};

#endif
