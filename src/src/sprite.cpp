#include "headers/sprite.h"

Sprite::Sprite()
{
  m_vertexBuffer = nullptr;
  m_indexBuffer = nullptr;
  m_Textures = nullptr;
}

Sprite::Sprite(const Sprite& other) {}

Sprite::~Sprite() {}

bool Sprite::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, char* spriteFilename, int renderX, int renderY)
{
  bool result;

  m_screenWidth = screenWidth;
  m_screenHeight = screenHeight;

  m_renderX = renderX;
  m_renderY = renderY;

  m_frameTime = 0;

  result = InitializeBuffers(device);
  if(!result)
    {
      return false;
    }

  result = LoadTextures(device, deviceContext, spriteFilename);
  if(!result)
    {
      return false;
    }

  return true;
}

void Sprite::Shutdown()
{
  ReleaseTextures();

  ShutdownBuffers();

  return;
}

bool Sprite::Render(ID3D11DeviceContext* deviceContext)
{
  bool result;

  result = UpdateBuffers(deviceContext);
  if(!result)
    {
      return false;
    }

  RenderBuffers(deviceContext);

  return true;
}

void Sprite::Update(float frameTime)
{
  m_frameTime += frameTime;

  if(m_frameTime >= m_cycleTime)
    {
      m_frameTime -= m_cycleTime;

      m_currentTexture++;

      if(m_currentTexture == m_textureCount)
	{
	  m_currentTexture = 0;
	}
    }

  return;
}

int Sprite::GetIndexCount()
{
  return m_indexCount;
}

ID3D11ShaderResourceView* Sprite::GetTexture()
{
  return m_Textures[m_currentTexture].GetTexture();
}

bool Sprite::InitializeBuffers(ID3D11Device* device)
{
  VertexType* vertices;
  unsigned long* indices;
  D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData, indexData;
  HRESULT result;
  int i;

  m_prevPosX = -1;
  m_prevPosY = -1;

  m_vertexCount = 6;

  m_indexCount = m_vertexCount;

  vertices = new VertexType[m_vertexCount];

  indices = new unsigned long[m_indexCount];

  memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

  for(i = 0; i < m_indexCount; i++)
    {
      indices[i] = i;
    }

  vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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
  indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
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
  vertices = nullptr;

  delete[] vertices;
  indices = nullptr;

  return true;
}

void Sprite::ShutdownBuffers()
{
  if(m_indexBuffer)
    {
      m_indexBuffer->Release();
      m_indexBuffer = nullptr;
    }

  if(m_vertexBuffer)
    {
      m_vertexBuffer->Release();
      m_vertexBuffer = nullptr;
    }

  return;
}

bool Sprite::UpdateBuffers(ID3D11DeviceContext* deviceContent)
{
  float left, right, top, bottom;
  VertexType* vertices;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  VertexType* dataPtr;
  HRESULT result;

  if((m_prevPosX == m_renderX) && (m_prevPosY == m_renderY))
    {
      return true;
    }

  m_prevPosX = m_renderX;
  m_prevPosY = m_renderY;

  vertices = new VertexType[m_vertexCount];

  left = (float)((m_screenWidth / 2) * -1) + (float)m_renderX;

  right = left + (float)m_bitmapWidth;

  top = (float)(m_screenHeight / 2) - (float)m_renderY;

  bottom = top - (float)m_bitmapHeight;

  vertices[0].position = XMFLOAT3(left, top, 0.0f);
  vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

  vertices[1].position = XMFLOAT3(right, bottom, 0.0f);
  vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

  vertices[2].position = XMFLOAT3(left, bottom, 0.0f);
  vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

  vertices[3].position = XMFLOAT3(left, top, 0.0f);
  vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

  vertices[4].position = XMFLOAT3(right, top, 0.0f);
  vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

  vertices[5].position = XMFLOAT3(right, bottom, 0.0f);
  vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

  result = deviceContent->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if(FAILED(result))
    {
      return false;
    }

  dataPtr = (VertexType*)mappedResource.pData;

  memcpy(dataPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

  deviceContent->Unmap(m_vertexBuffer, 0);

  dataPtr = 0;

  delete[] vertices;
  vertices = nullptr;

  return true;
}

void Sprite::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

bool Sprite::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
  char textureFilename[128];
  ifstream fin;
  int i, j;
  char input;
  bool result;

  fin.open(filename);
  if(fin.fail())
    {
      return false;
    }

  fin >> m_textureCount;

  m_Textures = new Texture[m_textureCount];

  fin.get(input);

  for(i = 0; i < m_textureCount; i++)
    {
      j = 0;
      fin.get(input);
      while(input != '\n')
	    {
	        textureFilename[j] = input;
	        j++;
	        fin.get(input);
	    }
      textureFilename[j] = '\0';

      result = m_Textures[i].Initialize(device, deviceContext, textureFilename);
      if(!result)
    	{
	      return false;
    	}
    }

  fin >> m_cycleTime;

  m_cycleTime = m_cycleTime * 0.001f;

  fin.close();

  m_bitmapWidth = m_Textures[0].GetWidth();
  m_bitmapHeight = m_Textures[0].GetHeight();

  m_currentTexture = 0;

  return true;
}

void Sprite::ReleaseTextures()
{
  int i;

  if(m_Textures)
    {
      for(i = 0; i < m_textureCount; i++)
	{
	  m_Textures[i].Shutdown();
	}

      delete[] m_Textures;
      m_Textures = nullptr;
    }

  return;
}

void Sprite::SetRenderLocation(int x, int y)
{
  m_renderX = x;
  m_renderY = y;
  return;
}

