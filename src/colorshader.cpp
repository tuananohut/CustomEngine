#include "headers/colorshader.h"

ColorShader::ColorShader()
{
	m_vertexShader = nullptr;
	m_hullShader = nullptr;
	m_domainShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_tessellationBuffer = nullptr;
}

ColorShader::ColorShader(const ColorShader& other) {}

ColorShader::~ColorShader() {}

bool ColorShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t hsFilename[128];
	wchar_t dsFilename[128];
	wchar_t psFilename[128];
	int error;

	error = wcscpy_s(vsFilename, 128, L"src/shaders/color.vs");
	if(error != 0)
	{
		return false;
	}


	error = wcscpy_s(hsFilename, 128, L"src/shaders/color.hs");
	if (error != 0)
	{
		return false;
	}


	error = wcscpy_s(dsFilename, 128, L"src/shaders/color.ds");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"src/shaders/color.ps");
	if(error != 0)
	{
		return false;
	}

	result = InitializeShader(device, hwnd, vsFilename, hsFilename, dsFilename, psFilename);
	if(!result)
	{
		return false;
	}

	return true;
}

void ColorShader::Shutdown()
{
	ShutdownShader();
}

bool ColorShader::Render(ID3D11DeviceContext* deviceContext,
						 int indexCount,
						 XMMATRIX worldMatrix,
						 XMMATRIX viewMatrix,
						 XMMATRIX projectionMatrix,
						 float tessellationAmount)
{
	bool result;

	result = SetShaderParamteres(deviceContext, worldMatrix, viewMatrix, projectionMatrix, tessellationAmount);
	if(!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);
	
	return true;
}

bool ColorShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* hullShaderBuffer = nullptr;
	ID3D10Blob* domainShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC tessellationBufferDesc;

	result = D3DCompileFromFile(vsFilename,
								NULL, NULL,
								"ColorVertexShader",
								"vs_5_0",
								D3D10_SHADER_ENABLE_STRICTNESS,
								0,
								&vertexShaderBuffer, &errorMessage);
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

	result = D3DCompileFromFile(hsFilename,
								NULL, NULL,
								"ColorHullShader",
								"hs_5_0",
								D3D10_SHADER_ENABLE_STRICTNESS,
								0,
								&hullShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, hsFilename);
		}
		else
		{
			MessageBox(hwnd, hsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = D3DCompileFromFile(dsFilename,
								NULL, NULL,
								"ColorDomainShader",
								"ds_5_0",
								D3D10_SHADER_ENABLE_STRICTNESS,
								0,
								&domainShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, dsFilename);
		}
		else
		{
			MessageBox(hwnd, dsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	
	result = D3DCompileFromFile(psFilename,
								NULL, NULL,
								"ColorPixelShader",
								"ps_5_0",
								D3D10_SHADER_ENABLE_STRICTNESS,
								0,
								&pixelShaderBuffer,
								&errorMessage);
	if(FAILED(result))
	{
		if(errorMessage)
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
	if(FAILED(result))
	{
		return false;
	}

	result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if(FAILED(result))
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

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if(FAILED(result))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = NULL;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = NULL;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&tessellationBufferDesc, NULL, &m_tessellationBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColorShader::ShutdownShader()
{
	if (m_tessellationBuffer)
	{
		m_tessellationBuffer->Release();
		m_tessellationBuffer = nullptr;
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

	if (m_domainShader)
	{
		m_domainShader->Release();
		m_domainShader = nullptr;
	}

	if (m_hullShader)
	{
		m_hullShader->Release();
		m_hullShader = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}
}

void ColorShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for(i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = NULL;

	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);
}

bool ColorShader::SetShaderParamteres(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float tessellationAmount)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	TessellationBufferType* dataPtr2;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	result = deviceContext->Map(m_tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr2 = (TessellationBufferType*)mappedResource.pData;

	dataPtr2->tessellationAmount = tessellationAmount;
	dataPtr2->padding = XMFLOAT3(0.f, 0.f, 0.f);

	deviceContext->Unmap(m_tessellationBuffer, 0);

	bufferNumber = 0;

	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_tessellationBuffer);

	return true;
}

void ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->HSSetShader(m_hullShader, NULL, 0);
	deviceContext->DSSetShader(m_domainShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}