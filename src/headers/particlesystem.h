#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include "texture.h"

using namespace DirectX;
using namespace std; 

class ParticleSystem
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT4 data1;
	};

	struct ParticleType
	{
		float positionX, positionY, positionZ;
		bool active;
		float lifeTime; 
		float scroll1X, scroll1Y; 
	};

public:
	ParticleSystem();
	ParticleSystem(const ParticleSystem&);
	~ParticleSystem();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();
	bool Frame(float, ID3D11DeviceContext*);
	void Render(ID3D11DeviceContext*);

	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();
	
	bool Reload(ID3D11Device*, ID3D11DeviceContext*);

private:
	bool LoadParticleConfiguration(); 

	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();
	void CopyParticle(int, int); 

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool UpdateBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*);
	void ReleaseTexture();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	ParticleType* m_particleList;
	VertexType* m_vertices;
	Texture* m_Texture;
	int m_vertexCount, m_indexCount;
	char m_configFilename[256];
	int m_maxParticles; 
	float m_particlesPerSecond; 
	float m_particleSize; 
	float m_particleLifeTime; 
	char m_textureFilename[256];
	float m_accumulatedTime; 
	int m_currentParticleCount; 
};

#endif