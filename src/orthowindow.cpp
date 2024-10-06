#include "headers/orthowindow.h"

OrthoWindow::OrthoWindow() 
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
}

OrthoWindow::OrthoWindow(const OrthoWindow& other) {}

OrthoWindow::~OrthoWindow() {}

bool OrthoWindow::Initialize(ID3D11Device* device, int windowWidth, int windowHeight)
{
	bool result;

	result = InitializeBuffers(device, windowWidth, windowHeight);
	if (!result)
	{
		return false;
	}

	return true;
}

void OrthoWindow::Shutdown()
{
	ShutdownBuffers();
}

void OrthoWindow::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int OrthoWindow::GetIndexCount()
{
	return m_indexCount;
}

bool OrthoWindow::InitializeBuffers(ID3D11Device* device, int windowWidth, int windowHeight) 
{
	float left, right, top, bottom;
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	left = (float)((windowWidth / 2) * -1);
	right = left + (float)windowWidth;
	top = (float)(windowHeight / 2);
	bottom = top - (float)windowHeight;

	m_vertexCount = 6;

	m_indexCount = m_vertexCount;

	vertices = new VertexType[m_vertexCount];

	indices = new unsigned long[m_indexCount];

	vertices[0].position = XMFLOAT3(left, top, 0.f);
	vertices[0].texture = XMFLOAT2(0.f, 0.f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.f);
	vertices[1].texture = XMFLOAT2(1.f, 1.f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.f);
	vertices[2].texture = XMFLOAT2(0.f, 1.f);

	vertices[3].position = XMFLOAT3(left, top, 0.f);
	vertices[3].texture = XMFLOAT2(0.f, 0.f);

	vertices[4].position = XMFLOAT3(right, top, 0.f);
	vertices[4].texture = XMFLOAT2(1.f, 0.f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.f);
	vertices[5].texture = XMFLOAT2(1.f, 1.f);

	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

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

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void OrthoWindow::ShutdownBuffers()
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
}

void OrthoWindow::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}