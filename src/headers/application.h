#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "d3d.h"
#include "input.h"
#include "timer.h"
#include "camera.h"
#include "model.h"
#include "light.h"
#include "orthowindow.h"
#include "rendertexture.h"
#include "blur.h"
#include "lightshader.h"
#include "heat.h"
#include "heatshader.h"

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
	bool RenderHeatToTexture();
	bool RenderSceneToTexture(float);
	bool Render(float);
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
	Model* m_Model;
	Light* m_Light;
	LightShader* m_LightShader; 
	RenderTexture* m_RenderTexture; 
	OrthoWindow* m_FullScreenWindow; 
	TextureShader* m_TextureShader;
	BlurShader* m_BlurShader;
	Blur* m_Blur;
	Heat* m_Heat; 
	HeatShader* m_HeatShader; 
	RenderTexture* m_HeatTexture; 
};

#endif
