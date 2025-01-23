#include "headers/particlesystem.h"

ParticleSystem::ParticleSystem()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_particleList = nullptr;
	m_vertices = nullptr;
	m_Texture = nullptr;
}

ParticleSystem::ParticleSystem(const ParticleSystem& other) {}

ParticleSystem::~ParticleSystem() {}

bool ParticleSystem::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* configFilename)
{
	bool result;

	strcpy_s(m_configFilename, configFilename);

	result = LoadParticleConfiguration();
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

	result = LoadTexture(device, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void ParticleSystem::Shutdown()
{
	ReleaseTexture();

	ShutdownBuffers();

	ShutdownParticleSystem();
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

bool ParticleSystem::LoadParticleConfiguration()
{
	ifstream fin;
	int i;
	char input;

	fin.open(m_configFilename);
	if (fin.fail())
	{
		return false;
	}

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_maxParticles;

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_particlesPerSecond;

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_particleSize;

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_particleLifeTime;

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);

	i = 0;
	fin.get(input);
	while (input != '\n')
	{
		m_textureFilename[i] = input;
		i++;
		fin.get(input);
	}
	m_textureFilename[i] = '\0';

	fin.close();

	return true;
}

bool ParticleSystem::InitializeParticleSystem()
{
	int i;

	m_particleList = new ParticleType[m_maxParticles];

	for (i = 0; i < m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	m_accumulatedTime = 0.f;

	m_currentParticleCount = 0;

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

void ParticleSystem::EmitParticles(float frameTime)
{
	float centerX, centerY, radius, positionX, positionY, positionZ, scroll1X, scroll1Y;
	float emitterOrigin[3];
	int index, i, j;
	bool emitParticle, found;
	static float angle = 0.f;

	centerX = 0.f;
	centerY = 0.f;

	radius = 1.f;

	angle += frameTime * 2.f;

	emitterOrigin[0] = centerX + radius * sin(angle);
	emitterOrigin[1] = centerY + radius * cos(angle);
	emitterOrigin[2] = 0.f;

	m_accumulatedTime += frameTime;

	emitParticle = false;

	if (m_accumulatedTime > (1.f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.f;
		emitParticle = true;
	}

	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		positionX = emitterOrigin[0];
		positionY = emitterOrigin[1];
		positionZ = emitterOrigin[2];

		scroll1X = (((float)rand() - (float)rand()) / RAND_MAX);
		if (scroll1X < 0.f)
		{
			scroll1X *= -1.f;
		}

		scroll1Y = scroll1X;

		index = 0;
		found = false;
		while (!found)
		{
			if ((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		i = m_currentParticleCount;
		j = i - 1;

		while (i != index)
		{
			CopyParticle(i, j);
			i--;
			j--;
		}

		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;
		m_particleList[index].active = true;
		m_particleList[index].lifeTime = m_particleLifeTime;
		m_particleList[index].scroll1X = scroll1X;
		m_particleList[index].scroll1Y = scroll1Y;
	}
}

void ParticleSystem::UpdateParticles(float frameTime)
{
	int i;

	for (i = 0; i < m_currentParticleCount; i++)
	{
		m_particleList[i].lifeTime = m_particleList[i].lifeTime - frameTime;

		m_particleList[i].scroll1X = m_particleList[i].scroll1X + (frameTime * 0.5f);
		if (m_particleList[i].scroll1X > 1.f)
		{
			m_particleList[i].scroll1X -= 1.f;
		}

		m_particleList[i].scroll1Y = m_particleList[i].scroll1Y + (frameTime * 0.5f);
		if (m_particleList[i].scroll1Y > 1.f)
		{
			m_particleList[i].scroll1Y -= 1.f;
		}
	}
}

void ParticleSystem::KillParticles()
{
	int i, j;

	for (i = 0; i < m_maxParticles; i++)
	{
		if ((m_particleList[i].active == true) && (m_particleList[i].lifeTime <= 0.0f))
		{
			m_particleList[i].active = false;
			m_currentParticleCount--;

			for (j = i; j < m_maxParticles - 1; j++)
			{
				CopyParticle(j, j + 1);
			}
		}
	}
}

void ParticleSystem::CopyParticle(int dst, int src)
{
	m_particleList[dst].positionX = m_particleList[src].positionX;
	m_particleList[dst].positionY = m_particleList[src].positionY;
	m_particleList[dst].positionZ = m_particleList[src].positionZ;
	m_particleList[dst].active = m_particleList[src].active;
	m_particleList[dst].lifeTime = m_particleList[src].lifeTime;
	m_particleList[dst].scroll1X = m_particleList[src].scroll1X;
	m_particleList[dst].scroll1Y = m_particleList[src].scroll1Y;
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

bool ParticleSystem::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	float lifeTime, scroll1X, scroll1Y;

	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	index = 0;

	for (i = 0; i < m_currentParticleCount; i++)
	{
		lifeTime = m_particleList[i].lifeTime / m_particleLifeTime;
		scroll1X = m_particleList[i].scroll1X;
		scroll1Y = m_particleList[i].scroll1Y;

		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.f, 1.f);
		m_vertices[index].data1 = XMFLOAT4(lifeTime, scroll1X, scroll1Y, 1.f);
		index++;

		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.f, 0.f);
		m_vertices[index].data1 = XMFLOAT4(lifeTime, scroll1X, scroll1Y, 1.f);
		index++;

		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.f, 1.f);
		m_vertices[index].data1 = XMFLOAT4(lifeTime, scroll1X, scroll1Y, 1.f);
		index++;

		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.f, 1.f);
		m_vertices[index].data1 = XMFLOAT4(lifeTime, scroll1X, scroll1Y, 1.f);
		index++;

		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.f, 0.f);
		m_vertices[index].data1 = XMFLOAT4(lifeTime, scroll1X, scroll1Y, 1.f);
		index++;

		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.f, 0.f);
		m_vertices[index].data1 = XMFLOAT4(lifeTime, scroll1X, scroll1Y, 1.f);
		index++;
	}

	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	verticesPtr = (VertexType*)mappedResource.pData;

	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

bool ParticleSystem::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	bool result;

	m_Texture = new Texture;

	result = m_Texture->Initialize(device, deviceContext, m_textureFilename);
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

bool ParticleSystem::Reload(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	bool result;

	Shutdown();

	result = LoadParticleConfiguration();
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

	result = LoadTexture(device, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}