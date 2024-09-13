#include "headers/reflectionshader.h"

ReflectionShader::ReflectionShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleState = nullptr;
	m_reflectionBuffer;
}


ReflectionShader::ReflectionShader(const ReflectionShader& other) {}

ReflectionShader::~ReflectionShader() {}

bool ReflectionShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error; 

	error = wcscpy_s(vsFilename, 128, L"../CustomEngine/src/shaders/reflection.vs");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"../CustomEngine/src/shaders/reflection.ps");
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

void ReflectionShader::Shutdown()
{
	ShutdownShader();
}

bool ReflectionShader::Render(ID3D11DeviceContext* deviceContext, 
							  int indexCount, 
							  XMMATRIX worldMatrix, 
							  XMMATRIX viewMatrix, 
							  XMMATRIX projectionMatrix,
							  ID3D11ShaderResourceView* texture,
							  ID3D11ShaderResourceView* reflectionTexture, 
							  XMMATRIX reflectionMatrix)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, reflectionTexture, reflectionMatrix);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool ReflectionShader::InitializeShader(ID3D11Device* device, 
										HWND hwnd, 
										WCHAR* vsFilename,
										WCHAR* psFilename)
{

}