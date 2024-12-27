#ifndef _DEFERREDBUFFERS_H_
#define _DEFERREDBUFFERS_H_

const int BUFFER_COUNT = 3;

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class DeferredBuffers
{
public:
	DeferredBuffers();
	DeferredBuffers(const DeferredBuffers&);
	~DeferredBuffers();

	bool Initialize(ID3D11Device*, int, int, float, float);
	void Shutdown();

	void SetRenderTargets(ID3D11DeviceContext*);
	void ClearRenderTargets(ID3D11DeviceContext*, float, float, float, float);

	ID3D11ShaderResourceView* GetShaderResourcePositions();
	ID3D11ShaderResourceView* GetShaderResourceNormals();
	ID3D11ShaderResourceView* GetShaderResourceColors();

	void GetProjectionMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	int GetTextureWidth();
	int GetTextureHeight();

private: 
	bool BuildRenderTexture(DXGI_FORMAT, int, ID3D11Device*);

private:
	int m_textureWidth, m_textureHeight;
	ID3D11Texture2D* m_renderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* m_renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* m_shaderResourceViewArray[BUFFER_COUNT];
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_orthoMatrix;
};

#endif