#pragma once

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <Windows.h>
#include <cmath>

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "light.h"
#include "rendertexture.h"
#include "lightshader.h"
#include "refractionshader.h"
#include "watershader.h"

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
  bool RenderRefractionToTexture();
  bool RenderReflectionToTexture();
  bool Render();

private:
  D3D* m_Direct3D;
  Camera* m_Camera;
  Model* m_GroundModel;
  Model* m_WallModel;
  Model* m_BathModel;
  Model* m_WaterModel;
  Light* m_Light;
  RenderTexture* m_RefractionTexture;
  RenderTexture* m_ReflectionTexture;
  LightShader* m_LightShader;
  RefractionShader* m_RefractionShader;
  WaterShader* m_WaterShader;
  float m_waterHeight, m_waterTranslation;
};

#endif
