#ifndef _HEAT_H_
#define _HEAT_H_

#include "d3d.h"
#include "texture.h"

class Heat
{
public:
	Heat();
	Heat(const Heat&);
	~Heat();

	bool Initialize(D3D*);
	void Shutdown();
	void Frame(float);

	ID3D11ShaderResourceView* GetTexture();
	void GetNoiseValues(XMFLOAT3&, XMFLOAT3&, XMFLOAT2&, XMFLOAT2&, XMFLOAT2&, float&, float&, float&, float&);

private:
	Texture* m_HeatNoiseTexture; 
	XMFLOAT3 m_scrollSpeeds, m_scales; 
	XMFLOAT2 m_distortion1, m_distortion2, m_distortion3;
	float m_distortionScale, m_distortionBias, m_emissiveMultiplier, m_noiseFrameTime;
};

#endif 