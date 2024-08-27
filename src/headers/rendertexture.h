#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class RenderTexture
{
public: 
	RenderTexture();
	RenderTexture(const RenderTexture&);
	~RenderTexture();

	bool Initialize(ID3D11Device*, int, int, float, float, int);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();

	void GetProjectionMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	int GetTextureWidth();
	int GetTextureHeight();

private:
	int m_textureWidth, m_textureHeight;
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_orthoMatrix;
};

#endif