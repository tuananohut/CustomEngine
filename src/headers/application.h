#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "light.h"
#include "rendertexture.h"
#include "depthshader.h"
#include "shadowshader.h"

const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.f;
const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;

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
	bool RenderDepthToTexture();
	bool Render(float);

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_CubeModel;
	Model* m_SphereModel;
	Model* m_GroundModel;
	Light* m_Light;
	RenderTexture* m_RenderTexture;
	DepthShader* m_DepthShader;
	ShadowShader* m_ShadowShader;
	float m_shadowMapBias;
};

#endif
