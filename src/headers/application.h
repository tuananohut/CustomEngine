#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "rendertexture.h"
#include "depthshader.h"
#include "blur.h"
#include "glowshader.h"

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
	bool RenderGlowToTexture(float);
	// bool SoundProcessing();
	// bool RenderDepthToTexture(XMMATRIX, XMMATRIX, XMMATRIX, RenderTexture*, Light*);
	// bool RenderBlackAndWhiteShadows();
	bool Render(float);

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_Model;
	RenderTexture* m_RenderTexture;
	RenderTexture* m_GlowTexture;
	OrthoWindow* m_FullScreenWindow;
	TextureShader* m_TextureShader;
	BlurShader* m_BlurShader;
	Blur* m_Blur;
	GlowShader* m_GlowShader;
};

#endif
