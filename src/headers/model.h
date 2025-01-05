#ifndef _MODEL_H_
#define _MODEL_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

#include "texture.h"

class Model
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	/*
	struct InstanceType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};
	*/

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};
	

public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*, char*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture(int);
	// int GetVertexCount();
	// int GetInstanceCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, char*, char*, char*);
	void ReleaseTextures();

	bool LoadModel(char*);
	void ReleaseModel();
	
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;
	Texture* m_Textures;
	ModelType* m_model;
};

#endif