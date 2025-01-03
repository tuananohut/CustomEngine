#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "deferredbuffers.h"
#include "gbuffershader.h"
#include "rendertexture.h"
#include "orthowindow.h"
#include "ssaoshader.h"
#include "ssaoblurshader.h"
#include "lightshader.h"
// #include "deferredshader.h"
// #include "normalmapshader.h"

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
	bool RenderGBuffer();
	bool RenderSSAO();
	bool BlurSSAOTexture();
	bool Render(float);
	// bool RenderSceneToTexture(float);
	// bool UpdateMouseStrings(int, int, bool);
	// bool TestIntersection(int, int);
	// bool RaySphereIntersect(XMFLOAT3, XMFLOAT3, float);

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Light* m_Light;
	Model* m_SphereModel, *m_GroundModel;
	DeferredBuffers* m_DeferredBuffers;
	GBufferShader* m_GBufferShader;
	RenderTexture* m_SSAORenderTexture;
	OrthoWindow* m_FullScreenWindow;
	SSAOShader* m_SSAOShader;
	Texture* m_RandomTexture;
	RenderTexture* m_BlurSSAORenderTexture;
	SSAOBlurShader* m_SSAOBlurShader;
	LightShader* m_LightShader;
	int m_screenWidth,m_screenHeight;
};

#endif
