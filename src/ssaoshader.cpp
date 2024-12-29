#include "headers/ssaoshader.h"

SSAOShader::SSAOShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleStateWrap = nullptr;
	m_sampleStateClamp = nullptr;
	m_ssaoBuffer = nullptr;
}

SSAOShader::SSAOShader(const SSAOShader& other) {}

SSAOShader::~SSAOShader() {}

bool SSAOShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	wchar_t vsFilename[128], psFilename[128];
	int error;
	bool result;

	error = wcscpy_s(vsFilename, 128, L"src/shaders/ssao.vs");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"src/shaders/ssao.ps");
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

void SSAOShader::Shutdown()
{
	ShutdownShader();
}

bool SSAOShader::Render(ID3D11DeviceContext* deviceContext,
						int indexCount,
						XMMATRIX worldMatrix,
						XMMATRIX viewMatrix,
						XMMATRIX projectionMatrix,
						ID3D11ShaderResourceView* positionTexture,
						ID3D11ShaderResourceView* normalTexture,
						ID3D11ShaderResourceView* randomTexture,
						float screenWidth, float screenHeight,
						float randomTextureSize,
						float sampleRadius,
						float ssaoScale,
						float ssaoBias,
						float ssaoIntensity)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, positionTexture, normalTexture, randomTexture, screenWidth, screenHeight, randomTextureSize, sampleRadius, ssaoScale, ssaoBias, ssaoIntensity);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool SSAOShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC ssaoBufferDesc;

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "SSAOVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}

		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK | MB_ICONERROR);
		}

		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "SSAOPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}

		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK | MB_ICONERROR);
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

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap);
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateClamp);
	if (FAILED(result))
	{
		return false;
	}

	ssaoBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ssaoBufferDesc.ByteWidth = sizeof(SSAOBufferType);
	ssaoBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ssaoBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ssaoBufferDesc.MiscFlags = 0;
	ssaoBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&ssaoBufferDesc, NULL, &m_ssaoBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void SSAOShader::ShutdownShader()
{
	if (m_ssaoBuffer)
	{
		m_ssaoBuffer->Release();
		m_ssaoBuffer = nullptr;
	}

	if (m_sampleStateWrap)
	{
		m_sampleStateWrap->Release();
		m_sampleStateWrap = nullptr;
	}

	if (m_sampleStateClamp)
	{
		m_sampleStateClamp->Release();
		m_sampleStateClamp = nullptr;
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

void SSAOShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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
	errorMessage = 0;

	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK | MB_ICONERROR);
}

bool SSAOShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
									 XMMATRIX worldMatrix,
									 XMMATRIX viewMatrix,
									 XMMATRIX projectionMatrix,
									 ID3D11ShaderResourceView* positionTexture,
									 ID3D11ShaderResourceView* normalTexture,
									 ID3D11ShaderResourceView* randomTexture,
									 float screenWidth,
									 float screenHeight,
									 float randomTextureSize,
									 float sampleRadius,
									 float ssaoScale,
									 float ssaoBias,
									 float ssaoIntensity)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	SSAOBufferType* dataPtr2;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	deviceContext->PSSetShaderResources(0, 1, &positionTexture);
	deviceContext->PSSetShaderResources(1, 1, &normalTexture);
	deviceContext->PSSetShaderResources(2, 1, &randomTexture);

	result = deviceContext->Map(m_ssaoBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr2 = (SSAOBufferType*)mappedResource.pData;

	dataPtr2->screenWidth = screenWidth;
	dataPtr2->screenHeight = screenHeight;
	dataPtr2->randomTextureSize = randomTextureSize;
	dataPtr2->sampleRadius = sampleRadius;
	dataPtr2->ssaoScale = ssaoScale;
	dataPtr2->ssaoBias = ssaoBias;
	dataPtr2->ssaoIntensity = ssaoIntensity;
	dataPtr2->padding = 0.f;

	deviceContext->Unmap(m_ssaoBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_ssaoBuffer);

	return true;
}

void SSAOShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &m_sampleStateWrap);
	deviceContext->PSSetSamplers(1, 1, &m_sampleStateClamp);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}