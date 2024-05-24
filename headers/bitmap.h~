#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <DirectXMath>
using namespace DirectX;

#include "texture.h"

class Bitmap
{
private:
  struct VertexType
  {
    XMFLOAT3 position;
    XMFLOAT2 texture;
  };

public:
  Bitmap();
  Bitmap(const Bitmap&);
  ~Bitmap();

  bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, char*, int, int);
  void Shutdown();
  bool Render(ID3D11DeviceContext*);

  int GetIndexCount();
  ID3D11ShaderResourceView* GetTexture();

  void SetRenderLocation(int, int);

private:
  bool InitializeBuffers(ID3D11Device*);
  void ShutdownBuffers();
  bool UpdateBuffers(ID3D11DeviceContext*);
  void RenderBuffers(ID3D11DeviceContext*);

  bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);;
  void ReleaseTexture();

private:
  ID3D11Buffer* *m_vertexBuffer, *m_indexBuffer;
  int m_vertexCount, m_indexCount, m_screenWidth, m_screenHeight, m_bitmapWidth, m_bitmapHeight, m_renderX, m_renderY, m_prevPosX, m_prevPosY;
  Texture* m_Texture;
};

#endif
