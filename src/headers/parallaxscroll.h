#ifndef _PARALLAXSCROLL_H_
#define _PARALLAXSCROLL_H_

#include <fstream>
using namespace std;

#include "texture.h"

class ParallaxScroll
{
private:
	struct ParallaxArrayType
	{
		float scrollSpeed;
		float textureTranslation;
		float textureOpacity; 
	};

public: 
	ParallaxScroll();
	ParallaxScroll(const ParallaxScroll&);
	~ParallaxScroll();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown(); 
	void Frame(float); 

	int GetTextureCount(); 
	ID3D11ShaderResourceView* GetTexture(int); 
	float GetTranslation(int); 
	float GetOpacity(int);

private:
	Texture* m_TextureArray; 
	ParallaxArrayType* m_ParallaxArray;
	int m_textureCount; 
};

#endif