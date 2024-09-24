#include "headers/fireshader.h"

FireShader::FireShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_noiseBuffer = nullptr;
	m_distortionBuffer = nullptr;
	m_sampleStateWrap = nullptr;
	m_sampleStateClamp = nullptr;
}

FireShader::FireShader(const FireShader& other) {}

FireShader::~FireShader() {}

bool FireShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	wchar_t vsFilename[128], psFilename[128];
	int error;
	bool result;

	error = wcscpy_s(vsFilename, 128, L"../CustomEngine/src/shaders/fire.vs");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"../CustomEngine/src/shaders/fire.ps");
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

void FireShader::Shutdown() 
{
	ShutdownShader();
}

bool FireShader::Render(ID3D11DeviceContext* deviceContext, 
						int indexCount, 
						XMMATRIX worldMatrix, 
						XMMATRIX viewMatrix, 
						XMMATRIX projectionMatrix, 
						ID3D11ShaderResourceView* fireTexture, 
						ID3D11ShaderResourceView* noiseTexture, 
						ID3D11ShaderResourceView* alphaTexture, 
						float frameTime, 
						XMFLOAT3 scrollSpeeds, 
						XMFLOAT3 scales, 
						XMFLOAT2 distortion1, 
						XMFLOAT2 distortion2,
						XMFLOAT2 distortion3, 
						float distortionScale, 
						float distortionBias)
{
	bool result; 

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, fireTexture, noiseTexture, alphaTexture, frameTime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool FireShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result; 
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC noiseBufferDesc;
	D3D11_BUFFER_DESC distortionBufferDesc;
	D3D11_SAMPLER_DESC samplerDescWrap;
	D3D11_SAMPLER_DESC samplerDescClamp;

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "FireVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}

		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_ICONERROR | MB_OK);
		}

		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "FirePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}

		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_ICONERROR | MB_OK);
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

	noiseBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	noiseBufferDesc.ByteWidth = sizeof(NoiseBufferType);
	noiseBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	noiseBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	noiseBufferDesc.MiscFlags = 0;
	noiseBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&noiseBufferDesc, NULL, &m_noiseBuffer);
	if (FAILED(result))
	{
		return false;
	}

	distortionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	distortionBufferDesc.ByteWidth = sizeof(DistortionBufferType);
	distortionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	distortionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	distortionBufferDesc.MiscFlags = 0;
	distortionBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&distortionBufferDesc, NULL, &m_distortionBuffer);
	if (FAILED(result))
	{
		return false;
	}

	samplerDescWrap.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescWrap.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescWrap.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescWrap.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescWrap.MipLODBias = 0.f;
	samplerDescWrap.MaxAnisotropy = 1;
	samplerDescWrap.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDescWrap.BorderColor[0] = 0;
	samplerDescWrap.BorderColor[1] = 0;
	samplerDescWrap.BorderColor[2] = 0;
	samplerDescWrap.BorderColor[3] = 0;
	samplerDescWrap.MinLOD = 0;
	samplerDescWrap.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDescWrap, &m_sampleStateWrap);
	if (FAILED(result))
	{
		return false;
	}

	samplerDescClamp.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescClamp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescClamp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescClamp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescClamp.MipLODBias = 0.f;
	samplerDescClamp.MaxAnisotropy = 1;
	samplerDescClamp.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDescClamp.BorderColor[0] = 0;
	samplerDescClamp.BorderColor[1] = 0;
	samplerDescClamp.BorderColor[2] = 0;
	samplerDescClamp.BorderColor[3] = 0;
	samplerDescClamp.MinLOD = 0;
	samplerDescClamp.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDescClamp, &m_sampleStateClamp);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void FireShader::ShutdownShader()
{
	if (m_sampleStateClamp)
	{
		m_sampleStateClamp->Release();
		m_sampleStateClamp = nullptr;
	}

	if (m_sampleStateWrap)
	{
		m_sampleStateWrap->Release();
		m_sampleStateWrap = nullptr;
	}

	if (m_distortionBuffer)
	{
		m_distortionBuffer->Release();
		m_distortionBuffer = nullptr;
	}

	if (m_noiseBuffer)
	{
		m_noiseBuffer->Release();
		m_noiseBuffer = nullptr;
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

void FireShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
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

bool FireShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
									 XMMATRIX worldMatrix, 
									 XMMATRIX viewMatrix, 
									 XMMATRIX projectionMatrix, 
									 ID3D11ShaderResourceView* fireTexture,
									 ID3D11ShaderResourceView* noiseTexture,
									 ID3D11ShaderResourceView* alphaTexture,
									 float frameTime,
									 XMFLOAT3 scrollSpeeds, 
									 XMFLOAT3 scales,
									 XMFLOAT2 distortion1,
									 XMFLOAT2 distortion2,
									 XMFLOAT2 distortion3,
									 float distortionScale,
									 float distortionBias)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	NoiseBufferType* dataPtr2;
	DistortionBufferType* dataPtr3;
	unsigned int bufferNumber;

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

	result = deviceContext->Map(m_noiseBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr2 = (NoiseBufferType*)mappedResource.pData;

	dataPtr2->frameTime = frameTime;
	dataPtr2->scrollSpeeds = scrollSpeeds;
	dataPtr2->scales = scales;
	dataPtr2->padding = 0.f;

	deviceContext->Unmap(m_noiseBuffer, 0);

	bufferNumber = 1;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_noiseBuffer);

	deviceContext->PSSetShaderResources(0, 1, &fireTexture);
	deviceContext->PSSetShaderResources(1, 1, &noiseTexture);
	deviceContext->PSSetShaderResources(2, 1, &alphaTexture);

	result = deviceContext->Map(m_distortionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr3 = (DistortionBufferType*)mappedResource.pData;

	dataPtr3->distortion1 = distortion1;
	dataPtr3->distortion2 = distortion2;
	dataPtr3->distortion3 = distortion3;
	dataPtr3->distortionScale = distortionScale;
	dataPtr3->distortionBias = distortionBias;
	
	deviceContext->Unmap(m_distortionBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_distortionBuffer);

	return true;
}

void FireShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &m_sampleStateWrap);
	deviceContext->PSSetSamplers(1, 1, &m_sampleStateClamp);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}