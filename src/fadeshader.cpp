#include "headers/fadeshader.h"

FadeShader::FadeShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleState = nullptr;
	m_fadeBuffer = nullptr;
}

FadeShader::FadeShader(const FadeShader& other) {}

FadeShader::~FadeShader() {}

bool FadeShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	wchar_t vsFilename[128], psFilename[128];
	int error;
	bool result;

	error = wcscpy_s(vsFilename, 128, L"../CustomEngine/src/shaders/fade.vs");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"../CustomEngine/src/shaders/fade.ps");
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

void FadeShader::Shutdown()
{
	ShutdownShader();
}

bool FadeShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float fadeAmount)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, fadeAmount);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);
	
	return false;
}

bool FadeShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) 
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr; 
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements; 
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC fadeBufferDesc;

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "FadeVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}

		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "FadePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}

		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}
	
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	fadeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fadeBufferDesc.ByteWidth = sizeof(FadeBufferType);
	fadeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fadeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fadeBufferDesc.MiscFlags = 0;
	fadeBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&fadeBufferDesc, NULL, &m_fadeBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void FadeShader::ShutdownShader()
{
	if (m_fadeBuffer)
	{
		m_fadeBuffer->Release();
		m_fadeBuffer = nullptr;
	}

	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = nullptr;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}
}

void FadeShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned __int64 bufferSize, i;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = nullptr;

	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK | MB_ICONERROR);
}

bool FadeShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float blendAmount)

{}