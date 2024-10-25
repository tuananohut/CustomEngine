#include "headers/particlesystem.h"

ParticleSystem::ParticleSystem()
{
	m_Texture = nullptr;
	m_particleList = nullptr;
	m_vertices = nullptr;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
}

ParticleSystem::ParticleSystem(const ParticleSystem& other) {}

ParticleSystem::~ParticleSystem() {}

bool ParticleSystem::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
	bool result;

	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result) 
	{
		return false;
	}

	result = InitializeParticleSystem();
	if (!result)
	{
		return false;
	}

	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void ParticleSystem::Shutdown() 
{
	ShutdownBuffers();

	ShutdownParticleSystem();

	ReleaseTexture();
}

bool ParticleSystem::Frame(float frameTime, ID3D11DeviceContext* deviceContext)
{
	bool result;

	KillParticles();

	EmitParticles(frameTime);

	UpdateParticles(frameTime);

	result = UpdateBuffers(deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void ParticleSystem::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

ID3D11ShaderResourceView* ParticleSystem::GetTexture()
{
	return m_Texture->GetTexture();
}

int ParticleSystem::GetIndexCount()
{
	return m_indexCount;
}

bool ParticleSystem::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	m_Texture = new Texture;

	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ParticleSystem::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}
}

bool ParticleSystem::InitializeParticleSystem() 
{
	int i;

	m_particleDeviationX = 0.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 2.f;

	m_particleVelocity = 1.f;
	m_particleVelocityVariation = 0.2f;

	m_particleSize = 0.2f;

	m_particlesPerSecond = 100.f;

	m_maxParticles = 1000;

	m_particleList = new ParticleType[m_maxParticles];

	for (i = 0; i < m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	m_currentParticleCount = 0;

	m_accumulatedTime = 0.f;

	return true;
}

void ParticleSystem::ShutdownParticleSystem()
{
	if (m_particleList)
	{
		delete[] m_particleList;
		m_particleList = nullptr;
	}
}

bool ParticleSystem::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = m_maxParticles * 6;

	m_indexCount = m_vertexCount;

	m_vertices = new VertexType[m_vertexCount];

	indices = new unsigned long[m_indexCount];

	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	delete[] indices;
	indices = nullptr;

	return true;
}

void ParticleSystem::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	if (m_vertices)
	{	
		delete[] m_vertices;
		m_vertices = nullptr;
	}
}

void ParticleSystem::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}