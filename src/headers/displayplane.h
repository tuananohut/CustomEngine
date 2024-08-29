#ifndef _DISPLAYPLANE_H_
#define _DISPLAYPLANE_H_

#include "d3d.h"

class DisplayPlane
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	DisplayPlane();
	DisplayPlane(const DisplayPlane&);
	~DisplayPlane();

	bool Initialize(ID3D11Device*, float, float);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*, float, float);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif 
