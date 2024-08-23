#include "headers/shadermanager.h"

ShaderManager::ShaderManager()
{
	m_TextureShader = nullptr;
	m_LightShader = nullptr;
	m_NormalMapShader = nullptr;
}

ShaderManager::ShaderManager(const ShaderManager& other) {}

ShaderManager::~ShaderManager() {}

bool ShaderManager::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	m_TextureShader = new TextureShader;

	result = m_TextureShader->Initialize(device, hwnd);
	if (!result)
	{
		return false;
	}

	m_LightShader = new LightShader;

	result = m_LightShader->Initialize(device, hwnd);
	if (!result)
	{
		return false;
	}

	m_NormalMapShader = new NormalMapShader;

	result = m_NormalMapShader->Initialize(device, hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void ShaderManager::Shutdown()
{
	if (m_NormalMapShader)
	{
		m_NormalMapShader->Shutdown();
		delete m_NormalMapShader;
		m_NormalMapShader = nullptr;
	}

	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = nullptr;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = nullptr;
	}
}

bool ShaderManager::RenderTextureShader(ID3D11DeviceContext* deviceContext,
										int indexCount,
										XMMATRIX worldMatrix,
										XMMATRIX viewMatrix,
										XMMATRIX projectionMatrix,	
										ID3D11ShaderResourceView* texture)
{
	bool result;

	result = m_TextureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderManager::RenderLightShader(ID3D11DeviceContext* deviceContext, 
									  int indexCount,
									  XMMATRIX worldMatrix,
									  XMMATRIX viewMatrix,
									  XMMATRIX projectionMatrix,
									  ID3D11ShaderResourceView* texture,
									  XMFLOAT3 lightDirection,
									  XMFLOAT4 diffuseColor)
{
	bool result;

	result = m_LightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderManager::RenderNormalMapShader(ID3D11DeviceContext* deviceContext, 
										  int indexCount, 
										  XMMATRIX worldMatrix,
										  XMMATRIX viewMatrix,
										  XMMATRIX projectionMatrix,
										  ID3D11ShaderResourceView* colorTexture,
										  ID3D11ShaderResourceView* normalTexture,
										  XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	bool result;

	result = m_NormalMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture, normalTexture, lightDirection, diffuseColor);
	if (!result)
	{
		return false;
	}

	return true;
}

