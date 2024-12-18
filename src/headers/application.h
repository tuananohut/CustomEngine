#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "orthowindow.h"
#include "deferredbuffers.h"
#include "deferredshader.h"
#include "lightshader.h"

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
	bool RenderSceneToTexture(float);
	bool Render(float);	
	// bool UpdateMouseStrings(int, int, bool);
	// bool TestIntersection(int, int);
	// bool RaySphereIntersect(XMFLOAT3, XMFLOAT3, float);

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Light* m_Light;
	Model* m_Model;
	OrthoWindow* m_FullScreenWindow; 
	DeferredBuffers* m_DeferredBuffers; 
	DeferredShader* m_DeferredShader; 
	LightShader* m_LightShader;
};

#endif
