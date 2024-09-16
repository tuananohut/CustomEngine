#include "headers/watershader.h"

WaterShader::WaterShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_sampleState = nullptr;
	m_matrixBuffer = nullptr;
	m_reflectionBuffer = nullptr;
	m_waterBuffer = nullptr;
}

WaterShader::WaterShader(const WaterShader& other) {}

WaterShader::~WaterShader() {}

bool WaterShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	error = wcscpy_s(vsFilename, 128, L"../CustomEngine/src/shaders/water.vs");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"../CustomEngine/src/shaders/water.ps");
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

void WaterShader::Shutdown()
{
	ShutdownShader();
}

bool WaterShader::Render(ID3D11DeviceContext* deviceContext, 
						 int indexCount, 
						 XMMATRIX worldMatrix, 
						 XMMATRIX viewMatirx, 
						 XMMATRIX projectionMatrix, 
						 ID3D11ShaderResourceView* reflectionTexture, 
						 ID3D11ShaderResourceView* refractionTexture, 
						 ID3D11ShaderResourceView* normalTexture, 
						 float waterTranslation, 
						 float reflectRefractScale)
{
	bool result; 

	result = SetShaderParameters(deviceContext, indexCount, worldMatrix, viewMatirx, projectionMatrix, reflectionTexture, refractionTexture, normalTexture, waterTranslation, reflectRefractScale);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool WaterShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{

}