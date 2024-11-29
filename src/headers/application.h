#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "light.h"
#include "rendertexture.h"
#include "depthshader.h"
#include "transparentdepthshader.h"
#include "shadowshader.h"

const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_NEAR = 0.3;
const float SCREEN_DEPTH = 1000.0f;

const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;
const float SHADOWMAP_NEAR = 1.f;
const float SHADOWMAP_DEPTH = 50.f;

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
	bool RenderSceneToTexture();
	// bool SoundProcessing();
	// bool RenderDepthToTexture(XMMATRIX, XMMATRIX, XMMATRIX, RenderTexture*, Light*);
	// bool RenderBlackAndWhiteShadows();
	bool Render(float);

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_TreeTrunkModel;
	Model* m_GroundModel;
	Model* m_TreeLeafModel;
	Light* m_Light;
	RenderTexture* m_RenderTexture;
	DepthShader* m_DepthShader;
	TransparentDepthShader* m_TransparentDepthShader;
	ShadowShader* m_ShadowShader;
	float m_shadowMapBias;
};

#endif
