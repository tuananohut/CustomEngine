#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

#include "texture.h"

class Sprite
{
 private:
  struct VertexType
  {
    XMFLOAT3 position;
    XMFLOAT2 texture;
  };
  
 public:
  Sprite();
  Sprite(const Sprite&);
  ~Sprite();

  bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, char*, int, int);
  void Shutdown();
  bool Render(ID3D11DeviceContext*);

  void Update(float);

  int GetIndexCount();
  ID3D11ShaderResourceView* GetTexture();

  void SetRenderLocation(int, int);

 private:
  bool InitializeBuffers(ID3D11Device*);
  void ShutdownBuffers();
  bool UpdateBuffers(ID3D11DeviceContext*);
  void RenderBuffers(ID3D11DeviceContext*);

  bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, char*);
  void ReleaseTextures();

 private:
  ID3D11Buffer* m_vertexBuffer;
  ID3D11Buffer* m_indexBuffer;
  int m_vertexCount, m_indexCount, m_screenWidth, m_screenHeight, m_bitmapWidth, m_bitmapHeight, m_renderX, m_renderY, m_prevPosX, m_prevPosY;
  Texture* m_Textures;
  float m_frameTime, m_cycleTime;
  int m_currentTexture, m_textureCount;
};

#endif
