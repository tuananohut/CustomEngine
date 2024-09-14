#pragma once

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <Windows.h>
#include <cmath>

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "textureshader.h"
#include "rendertexture.h"
#include "reflectionshader.h"
#include "colorshader.h"


const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

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
  bool RenderReflectionToTexture(float);
  bool Render(float);

private:
  D3D* m_Direct3D;
  Camera* m_Camera;
  Model* m_CubeModel;
  Model* m_FloorModel;
  Model* m_ConeModel;
  Model* m_SphereModel;
  RenderTexture* m_RenderTexture;
  TextureShader* m_TextureShader;
  ReflectionShader* m_ReflectionShader;
  ColorShader* m_ColorShader;
};

#endif
