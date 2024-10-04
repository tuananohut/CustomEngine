#ifndef _BLUR_H_
#define _BLUR_H_

#include "d3d.h"
#include "camera.h"
#include "rendertexture.h"
#include "orthowindow.h"
#include "textureshader.h"
#include "blurshader.h"

class Blur
{
public:
	Blur();
	Blur(const Blur&);
	~Blur();

	bool Initialize(D3D*, int, int, float, float, int, int);
	void Shutdown();

	bool BlurTexture(D3D*, Camera*, RenderTexture*, TextureShader*, BlurShader*);

private:
	RenderTexture* m_DownSampleTexture1, *m_DownSampleTexture2;
	OrthoWindow* m_DownSampleWindow, *m_UpSampleWindow;
	int m_downSampleWidth, m_downSampleHeight;
};


#endif