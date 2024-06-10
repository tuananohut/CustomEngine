#ifndef _FONT_H_
#define _FONT_H_

#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

#include "texture.h"

class Font
{
 private:
  struct FontType
  {
    float left, right;
    int size;
  };

  struct VertexType
  {
    XMFLOAT3 position;
    XMFLOAT2 texture;
  };

 public:
  Font();
  Font(const Font&);
  ~Font();

  bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int);
  void Shutdown();

  ID3D11ShaderResourceView* GetTexture();

  void BuildVertexArray(void*, char*, float, float);
  int GetSentencePixelLength(char*);
  int GetFontHeight();

 private:
  bool LoadFontData(char*);
  void ReleaseFontData();
  bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
  void ReleaseTexture();

 private:
  FontType* m_Font;
  Texture* m_Texture;
  float m_fontHeight;
  int m_spaceSize;
};

#endif
