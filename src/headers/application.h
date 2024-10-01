#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <Windows.h>
#include <cmath>

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "model.h"
// #include "position.h"
// #include "textureshader.h"
// #include "timer.h"

#include "depthshader.h"

/* Trying wave file player */
#include "xaudio.h"
#include "xaudiosound.h"
#include "xaudiosound3d.h"

const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.f;

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
	// bool RenderSceneToTexture(float);
	bool SoundProcessing();
	bool Render(float);

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_Model;
	// Position* m_Position;
	// Timer* m_Timer;
	// TextureShader* m_TextureShader;

	DepthShader* m_DepthShader;

	XAudio* m_XAudio;
	XAudioSound3D* m_TestSound2;
};

#endif
