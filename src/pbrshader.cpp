#include "headers/pbrshader.h"

PBRShader::PBRShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_sampleState = nullptr;
	m_matrixBuffer = nullptr;
	m_cameraBuffer = nullptr; 
	m_lightBuffer = nullptr;
}

PBRShader::PBRShader(const PBRShader& other) {}

PBRShader::~PBRShader() {}

bool PBRShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	wchar_t vsFilename[128], psFilename[128];
	bool result;
	int error;

	error = wcscpy_s(vsFilename, 128, L"src/shaders/pbr.vs");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"src/shaders/pbr.ps");
	if (error != 0)
	{
		return false;
	}

	error = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (error != 0)
	{
		return false;
	}
	
	return true;
}

void PBRShader::Shutdown()
{
	ShutdownShader();
}

bool PBRShader::Render(ID3D11DeviceContext* deviceContext,
					   int indexCount,
					   XMMATRIX worldMatrix,
					   XMMATRIX viewMatrix,
					   XMMATRIX projectionMatrix,
					   ID3D11ShaderResourceView* diffuseTexture,
					   ID3D11ShaderResourceView* normalMap,
	                   ID3D11ShaderResourceView* rmTexture,
					   XMFLOAT3 lightDirection,
					   XMFLOAT3 cameraPosition)
{
	bool result; 

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, diffuseTexture, normalMap, rmTexture, lightDirection, cameraPosition);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool PBRShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result; 
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements; 
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc; 
	D3D11_BUFFER_DESC lightBufferDesc;


}