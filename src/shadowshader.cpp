#include "headers/shadowshader.h"

ShadowShader::ShadowShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleStateClamp = nullptr;
	m_sampleStateWrap = nullptr;
	m_lightPositionBuffer = nullptr;
	m_lightBuffer = nullptr;
}

ShadowShader::ShadowShader(const ShadowShader& other) {}

ShadowShader::~ShadowShader() {}

bool ShadowShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	wchar_t vsFilename[128], psFilename[128];
	int error;
	bool result;

	error = wcscpy_s(vsFilename, 128, L"../CustomEngine/src/shaders/shadow.vs");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"../CustomEngine/src/shaders/shadow.ps");
	if (error != 0)
	{
		return false;
	}

	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ShadowShader::Shutdown()
{
	ShutdownShader();
}

bool ShadowShader::Render(ID3D11DeviceContext* deviceContext,
						  int indexCount,
						  XMMATRIX worldMatrix,
						  XMMATRIX viewMatrix,
						  XMMATRIX projectionMatrix,
						  XMMATRIX lightViewMatrix,
						  XMMATRIX lightProjectionMatrix,
					      ID3D11ShaderResourceView* texture,
						  ID3D11ShaderResourceView* depthMapTexture,
						  XMFLOAT4 ambientColor, 
						  XMFLOAT4 diffuseColor, 
						  XMFLOAT3 lightPosition, 
						  float bias)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, texture, depthMapTexture, ambientColor, diffuseColor, lightPosition, bias);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool ShadowShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	
}


