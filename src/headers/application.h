#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "light.h"
#include "lightshader.h"
#include "fontshader.h"
#include "font.h"
#include "text.h"
#include "bitmap.h"
#include "textureshader.h"

const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_NEAR = 0.3;
const float SCREEN_DEPTH = 1000.0f;

class Application
{
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(Input*);

private:
	bool Render(float);	
	bool UpdateMouseStrings(int, int, bool);
	bool TestIntersection(int, int);
	bool RaySphereIntersect(XMFLOAT3, XMFLOAT3, float);

private:
	D3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_Model;
	Light* m_Light;
	LightShader* m_LightShader;
	FontShader* m_FontShader;
	Font* m_Font;
	Text* m_Text;
	Text* m_MouseStrings; 
	Bitmap* m_MouseBitmap;
	TextureShader* m_TextureShader;
	int m_screenWidth, m_screenHeight;
};

#endif
