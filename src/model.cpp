#include "headers/model.h"

Model::Model()
{
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;
	m_Texture = NULL;
	m_model = NULL;
}

Model::Model(const Model& other) {}

Model::~Model() {}

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* textureFilename)
{
	bool result;

	result = LoadModel(modelFilename);
	if(!result)
	{
		return false;
	}

	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	result = LoadTexture(device, deviceContext, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}

void Model::Shutdown()
{
	ReleaseModel();

	ReleaseTexture();

	ShutdownBuffers();

	return;
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

int Model::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return m_Texture->GetTexture();
}

bool Model::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// m_vertexCount = 4;
	// 
	// m_indexCount = 6;
	// 
	// vertices = new VertexType[m_vertexCount];
	// if(!vertices)
	// {
	// 	return false;
	// }
	// 
	// indices = new unsigned long[m_indexCount];
	// if(!indices)
	// {
	// 	return false;
	// }

	vertices = new VertexType[m_vertexCount];

	indices = new unsigned long[m_indexCount];

	for(i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// Triangle code - Color
	// vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	// vertices[0].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	// 
	// vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	// vertices[1].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	// 
	// vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	// vertices[2].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	// 
	// indices[0] = 0;
	// indices[1] = 1;
	// indices[2] = 2;

	// Square code - Color
	// vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	// vertices[0].color = XMFLOAT4(1.0f , 0.0f, 1.0f, 1.0f);
	// 
	// vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	// vertices[1].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	// 
	// vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	// vertices[2].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	// 
	// vertices[3].position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	// vertices[3].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	// 
	// indices[0] = 0;
	// indices[1] = 1;
	// indices[2] = 2;
	// indices[3] = 1;
	// indices[4] = 3;
	// indices[5] = 2;

	// Triangle Code - Texture
	// vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	// vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	// vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	// 
	// vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	// vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	// vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	// 
	// 
	// vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	// vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	// vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	// 
	// indices[0] = 0;
	// indices[1] = 1;
	// indices[2] = 2;

	// Square Code - Texture
	// vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	// vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	// 
	// 
	// vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	// vertices[1].texture = XMFLOAT2(0.0f, 0.0f);
	// 
	// 
	// vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	// vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	// 
	// 
	// vertices[3].position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	// vertices[3].texture = XMFLOAT2(1.0f, 0.0f);
	// 
	// indices[0] = 0;
	// indices[1] = 1;
	// indices[2] = 2;
	// indices[3] = 1;
	// indices[4] = 3;
	// indices[5] = 2;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) *m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	delete[] vertices;
	vertices = NULL;

	delete[] indices;
	indices = 0;

	return true;
}

void Model::ShutdownBuffers()
{
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = NULL;
	}

	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = NULL;
	}

	return;
}

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool Model::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	m_Texture = new Texture;

	result = m_Texture->Initialize(device, deviceContext, filename);
	
	if(!result)
	{
		return false;
	}

	return true;
}

void Model::ReleaseTexture()
{
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = NULL;
	}

	return;
}

bool Model::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;

	fin.open(filename);

	if(fin.fail())
	{
		return false;
	}

	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}

	fin >> m_vertexCount;

	m_indexCount = m_vertexCount;

	m_model = new ModelType[m_vertexCount];

	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	for(i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	fin.close();

	return true;
}

void Model::ReleaseModel()
{
	if(m_model)
	{
		delete[] m_model;
		m_model = NULL;
	}

	return;
}