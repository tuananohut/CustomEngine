#include "headers/glowshader.h"

GlowShader::GlowShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_glowBuffer = nullptr;
	m_sampleState = nullptr;
}

GlowShader::GlowShader(const GlowShader&) {}

GlowShader::~GlowShader() {}

bool GlowShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error; 

	error = wcscpy_s(vsFilename, 128, L"src/shaders/glow.vs");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"src/shaders/glow.ps");
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

void GlowShader::Shutdown()
{
	ShutdownShader();
}

bool GlowShader::Render(ID3D11DeviceContext* deviceContext, 
						int indexCount, 
						XMMATRIX worldMatrix,
						XMMATRIX viewMatrix, 
						XMMATRIX projectionMatrix, 
						ID3D11ShaderResourceView* texture,
						ID3D11ShaderResourceView* glowTexture, 
						float glowStrength)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, glowTexture, glowStrength);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool GlowShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{

}
