#include "headers/parallaxscroll.h"

ParallaxScroll::ParallaxScroll()
{
	m_TextureArray = nullptr; 
	m_ParallaxArray = nullptr; 
}

ParallaxScroll::ParallaxScroll(const ParallaxScroll& other) {}

ParallaxScroll::~ParallaxScroll() {}

bool ParallaxScroll::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* configFilename)
{
	ifstream fin;
	char textureFilename[256];
	int i, j; 
	char input; 
	float scrollSpeed;
	float opacity; 
	bool result; 

	fin.open(configFilename);
	if (!fin.good())
	{
		return false; 
	}

	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}
	fin >> m_textureCount;
	
	fin.get(input); 

	if ((m_textureCount < 1) || (m_textureCount > 64))
	{
		return false; 
	}

	m_TextureArray = new Texture[m_textureCount]; 

	m_ParallaxArray = new ParallaxArrayType[m_textureCount];

	for (i = 0; i < m_textureCount; i++)
	{
		j = 0;
		fin.get(input); 

		while(input != ' ')
		{
			textureFilename[j] = input; 
			j++;
			fin.get(input);
		}
		textureFilename[j] = '\0';

		fin >> scrollSpeed; 

		fin.get(input); 

		fin >> opacity;

		fin.get(input);

		result = m_TextureArray[i].Initialize(device, deviceContext, textureFilename);
		if (!result)
		{
			return false; 
		}

		m_ParallaxArray[i].scrollSpeed = scrollSpeed;
		m_ParallaxArray[i].textureOpacity = opacity; 
		m_ParallaxArray[i].textureTranslation = 0.f;
	}

	fin.close();

	return true;
}

void ParallaxScroll::Shutdown()
{
	int i;

	if (m_ParallaxArray)
	{
		delete[] m_ParallaxArray;
		m_ParallaxArray = nullptr; 
	}
	
	if (m_TextureArray)
	{
		for (i = 0; i < m_textureCount; i++)
		{
			m_TextureArray[i].Shutdown();
		}

		delete[] m_TextureArray;
		m_TextureArray = nullptr; 
	}
}

void ParallaxScroll::Frame(float frameTime)
{
	int i;

	for (i = 0; i < m_textureCount; i++)
	{
		m_ParallaxArray[i].textureTranslation += frameTime * m_ParallaxArray[i].scrollSpeed;
		if (m_ParallaxArray[i].textureTranslation > 1.f)
		{
			m_ParallaxArray[i].textureTranslation -= 1.f; 
		}
	}
}

int ParallaxScroll::GetTextureCount()
{
	return m_textureCount;
}

ID3D11ShaderResourceView* ParallaxScroll::GetTexture(int index)
{
	return m_TextureArray[index].GetTexture();
}

float ParallaxScroll::GetTranslation(int index)
{
	return m_ParallaxArray[index].textureTranslation;
}

float ParallaxScroll::GetOpacity(int index)
{
	return m_ParallaxArray[index].textureOpacity;
}