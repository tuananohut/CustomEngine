#pragma once

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <Windows.h>
#include <cmath>

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "rendertexture.h"
#include "textureshader.h"
#include "glassshader.h"


/* Trying wave file player */
#include "directsound.h"
#include "sound.h"

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
	bool RenderSceneToTexture(float);
	bool Render(float);

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_Model;
	Model* m_WindowModel;
	RenderTexture* m_RenderTexture;
	TextureShader* m_TextureShader;
	GlassShader* m_GlassShader;

	DirectSound* m_DirectSound;
	Sound* m_TestSound1;
};

#endif
