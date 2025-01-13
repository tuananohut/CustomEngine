#include "headers/heat.h"

Heat::Heat() 
{
	m_HeatNoiseTexture = nullptr; 
}

Heat::Heat(const Heat& other) {}

Heat::~Heat() {}

bool Heat::Initialize(D3D* Direct3D)
{
	char textureFilename[128];
	bool result; 

	m_HeatNoiseTexture = new Texture;

	strcpy_s(textureFilename, "assets/textures/heatnoise01.tga");

	result = m_HeatNoiseTexture->Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), textureFilename);
	if (!result)
	{
		return false; 
	}

	m_scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f);

	m_scales = XMFLOAT3(1.f, 2.f, 3.f);

	m_distortion1 = XMFLOAT2(0.1f, 0.2f);
	m_distortion2 = XMFLOAT2(0.1f, 0.3f);
	m_distortion3 = XMFLOAT2(0.1f, 0.1f);

	m_distortionScale = 0.8f;
	m_distortionBias = 0.5f;
	m_emissiveMultiplier = 1.6f; 
	m_noiseFrameTime = 0.f; 

	return true;
}

void Heat::Shutdown()
{
	if (m_HeatNoiseTexture)
	{
		m_HeatNoiseTexture->Shutdown();
		delete m_HeatNoiseTexture;
		m_HeatNoiseTexture = nullptr; 
	}
}

void Heat::Frame(float frameTime)
{
	m_noiseFrameTime += (frameTime * 0.075f);
	if (m_noiseFrameTime > 1000.f)
	{
		m_noiseFrameTime = 0.f; 
	}
}

ID3D11ShaderResourceView* Heat::GetTexture()
{
	return m_HeatNoiseTexture->GetTexture();
}

void Heat::GetNoiseValues(XMFLOAT3& scrollSpeeds, 
						  XMFLOAT3& scales, 
						  XMFLOAT2& distortion1,
						  XMFLOAT2& distortion2,
						  XMFLOAT2& distortion3,
						  float& distortionScale, 
						  float& distortionBias,
						  float& emissiveMultiplier,
						  float& noiseFrameTime)
{
	scrollSpeeds = m_scrollSpeeds;
	scales = m_scales;
	distortion1 = m_distortion1;
	distortion2 = m_distortion2;
	distortion3 = m_distortion3;
	distortionScale = m_distortionScale; 
	distortionBias = m_distortionBias;
	emissiveMultiplier = m_emissiveMultiplier;
	noiseFrameTime = m_noiseFrameTime; 
}