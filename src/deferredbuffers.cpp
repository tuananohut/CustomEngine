#include "headers/deferredbuffers.h"

DeferredBuffers::DeferredBuffers()
{
	int i = 0; 

	for (i = 0; i < BUFFER_COUNT; i++)
	{
		m_renderTargetTextureArray[i] = nullptr;
		m_renderTargetViewArray[i] = nullptr;
		m_shaderResourceViewArray[i] = nullptr;
	}

	m_depthStencilBuffer = nullptr;
	m_depthStencilView = nullptr;
}

DeferredBuffers::DeferredBuffers(const DeferredBuffers& other) {}

DeferredBuffers::~DeferredBuffers() {}

bool DeferredBuffers::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	bool result2;

	m_textureWidth = textureWidth;
	m_textureHeight = textureHeight;

	// Positions
	result2 = BuildRenderTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, 0, device);
	if (!result2)
	{
		return false;
	}

	// Normals
	result2 = BuildRenderTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, 1, device); 
	if (!result2)
	{
		return false;
	}

	// Colors
	result2 = BuildRenderTexture(DXGI_FORMAT_R8G8B8A8_UNORM, 2, device);
	if (!result2)
	{
		return false;
	}

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	m_viewport.Width = (float)textureWidth;
	m_viewport.Height = (float)textureHeight;
	m_viewport.MinDepth = 0.f;
	m_viewport.MaxDepth = 1.f;
	m_viewport.TopLeftX = 0.f; 
	m_viewport.TopLeftY = 0.f; 

	m_projectionMatrix = XMMatrixPerspectiveFovLH((3.141592654f / 4.0f), ((float)textureWidth / (float)textureHeight), screenNear, screenDepth);

	m_orthoMatrix = XMMatrixOrthographicLH((float)textureWidth, (float)textureHeight, screenNear, screenDepth);

	return true;
}

void DeferredBuffers::Shutdown()
{
	int i;

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = nullptr;
	}

	for (i = 0; i < BUFFER_COUNT; i++)
	{
		if (m_shaderResourceViewArray[i])
		{
			m_shaderResourceViewArray[i]->Release();
			m_shaderResourceViewArray[i] = nullptr;
		}

		if (m_renderTargetViewArray[i])
		{
			m_renderTargetViewArray[i]->Release();
			m_renderTargetViewArray[i] = nullptr;
		}

		if (m_renderTargetTextureArray[i])
		{
			m_renderTargetTextureArray[i]->Release();
			m_renderTargetTextureArray[i] = nullptr;
		}
	}
}

void DeferredBuffers::SetRenderTargets(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetRenderTargets(BUFFER_COUNT, m_renderTargetViewArray, m_depthStencilView);

	deviceContext->RSSetViewports(1, &m_viewport);
}

void DeferredBuffers::ClearRenderTargets(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha)
{
	float color[4];
	int i;

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	for (i = 0; i < BUFFER_COUNT; i++)
	{
		deviceContext->ClearRenderTargetView(m_renderTargetViewArray[i], color);
	}

	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}

bool DeferredBuffers::BuildRenderTexture(DXGI_FORMAT rtFormat, int arraySlot, ID3D11Device* device)
{
	HRESULT result; 
	D3D11_TEXTURE2D_DESC textureDesc; 
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc; 
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc; 

	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = m_textureWidth; 
	textureDesc.Height = m_textureHeight; 
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1; 
	textureDesc.Format = rtFormat;
	textureDesc.SampleDesc.Count = 1; 
	textureDesc.Usage = D3D11_USAGE_DEFAULT; 
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0; 
	textureDesc.MiscFlags = 0;

	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTextureArray[arraySlot]);
	if (FAILED(result))
	{
		return false;
	}

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; 
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateRenderTargetView(m_renderTargetTextureArray[arraySlot], &renderTargetViewDesc, &m_renderTargetViewArray[arraySlot]);
	if (FAILED(result))
	{
		return false;
	}

	shaderResourceViewDesc.Format = textureDesc.Format; 
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	result = device->CreateShaderResourceView(m_renderTargetTextureArray[arraySlot], &shaderResourceViewDesc, &m_shaderResourceViewArray[arraySlot]);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void DeferredBuffers::GetProjectionMatrix(XMMATRIX& projectionMatrix) 
{
	projectionMatrix = m_projectionMatrix;
}

void DeferredBuffers::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

int DeferredBuffers::GetTextureWidth()
{
	return m_textureWidth;
}

int DeferredBuffers::GetTextureHeight()
{
	return m_textureHeight;
}

ID3D11ShaderResourceView* DeferredBuffers::GetShaderResourcePositions()
{
	return m_shaderResourceViewArray[0];
}

ID3D11ShaderResourceView* DeferredBuffers::GetShaderResourceNormals()
{
	return m_shaderResourceViewArray[1];
}

ID3D11ShaderResourceView* DeferredBuffers::GetShaderResourceColors()
{
	return m_shaderResourceViewArray[2];
}