#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <d3d11.h>
#include <stdio.h>

class Texture
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height; 
		unsigned char bpp;
		unsigned char data2;
	};

public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	int GetWidth();
	int GetHeight();

private:
	bool LoadTarga32Bit(char*);

private:
	unsigned char* m_targaData;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;
	int m_width, m_height;
};

#endif 