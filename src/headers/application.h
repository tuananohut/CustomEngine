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
#include "blur.h"
#include "blurshader.h"

/* */

#include "position.h"
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
	bool RenderSceneToTexture(float, float);
	// bool SoundProcessing();
	bool Render(float);

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_Model;
	TextureShader* m_TextureShader;
	RenderTexture* m_RenderTexture;
	OrthoWindow* m_FullScreenWindow;
	Blur* m_Blur;
	BlurShader* m_BlurShader;

	// Position* m_Position;
	// Timer* m_Timer;
};

#endif
