#include "headers/lightmapshader.h"

LightMapShader::LightMapShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleState = nullptr;
}

LightMapShader::LightMapShader(const LightMapShader& othetr) {}

LightMapShader::~LightMapShader() {}

bool LightMapShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	error = wcscpy_s(vsFilename, 128, L"lights/lightmap.vs");
	if(error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"lights/lightmap.ps");
	if(error != 0)
	{
		return false;
	}

	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if(!result)
	{
		return false;
	}

	return true;
}	

void LightMapShader::Shutdown()
{
	ShutdownShader();
}

bool LightMapShader::Render(ID3D11DeviceContext* deviceContext,
							int indexCount,
							XMMATRIX worldMatrix,
							XMMATRIX viewMatrix,
							XMMATRIX projectionMatrix,
							ID3D11ShaderResourceView* texture1,
							ID3D11ShaderResourceView* texture2)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture1, texture2);
	if(!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool LightMapShader::InitializeShader(ID3D11Device* device,
									  HWND hwnd, 
									  WCHAR* vsFilename,
									  WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "LightMapVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if(FAILED(result))
	{
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// PIXEL SHADER ...

}