#include "headers/normalmapshader.h"

NormalMapShader::NormalMapShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_lightBuffer = nullptr;
	m_sampleState = nullptr;
}

NormalMapShader::NormalMapShader(const NormalMapShader& other) {}

NormalMapShader::~NormalMapShader() {}

bool NormalMapShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	error = wcscpy_s(vsFilename, 128, L"../CustomEngine/src/textures/normalmap.vs");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"../CustomEngine/src/textures/normalmap.ps");
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

void NormalMapShader::Shutdown()
{
	ShutdownShader();
}

bool NormalMapShader::Render(ID3D11DeviceContext* deviceContext, 
							 int indexCount,
							 XMMATRIX worldMatrix,
							 XMMATRIX viewMatrix,
							 XMMATRIX projectionMatrix,
							 ID3D11ShaderResourceView* texture1,
							 ID3D11ShaderResourceView* texture2, 
	                         XMFLOAT3 lightDirection, 
							 XMFLOAT4 diffuseColor)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture1, texture2, lightDirection, diffuseColor);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool NormalMapShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

}