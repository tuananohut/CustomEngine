#include "headers/rendertexture.h"

RenderTexture::RenderTexture()
{
	m_renderTargetTexture = nullptr;
	m_renderTargetView = nullptr;
	m_shaderResourceView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilView = nullptr;
}

RenderTexture::RenderTexture(const RenderTexture& other) {}

RenderTexture::~RenderTexture() {}

bool RenderTexture::Initialize(ID3D11Device* device,
	                           int textureWidth, int textureHeight,
							   float screenDepth,
							   float screenNear,
							   int format)
{
	
}