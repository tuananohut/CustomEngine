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

}