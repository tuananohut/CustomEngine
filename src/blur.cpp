#include "headers/blur.h"

Blur::Blur() 
{
	m_DownSampleTexture1 = nullptr;
	m_DownSampleTexture2 = nullptr;
	m_DownSampleWindow = nullptr;
	m_UpSampleWindow = nullptr;
}

Blur::Blur(const Blur& other) {}

Blur::~Blur() {}

bool Blur::Initialize(D3D* Direct3D, int downSampleWidth, int downSampleHeight, float screenNear, float screenDepth, int renderWidth, int renderHeight)
{
	bool result;

	m_downSampleWidth = downSampleWidth;
	m_downSampleHeight = downSampleHeight;

	m_DownSampleTexture1 = new RenderTexture;
	result = m_DownSampleTexture1->Initialize(Direct3D->GetDevice(), m_downSampleWidth, m_downSampleHeight, screenDepth, screenNear, 1);
	if (!result)
	{
		return false;
	}

	m_DownSampleTexture2 = new RenderTexture;
	result = m_DownSampleTexture2->Initialize(Direct3D->GetDevice(), m_downSampleWidth, m_downSampleHeight, screenDepth, screenNear, 1);
	if (!result)
	{
		return false;
	}

	m_DownSampleWindow = new OrthoWindow;
	result = m_DownSampleWindow->Initialize(Direct3D->GetDevice(), m_downSampleWidth, m_downSampleHeight);
	if (!result)
	{
		return false;
	}

	m_UpSampleWindow = new OrthoWindow;
	result = m_UpSampleWindow->Initialize(Direct3D->GetDevice(), m_downSampleWidth, m_downSampleHeight);
	if (!result)
	{
		return false;
	}

	return true;
}

/*  */

