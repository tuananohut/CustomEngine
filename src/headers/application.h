#pragma once

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <Windows.h>
#include <cmath>

#include "d3d.h"
#include "camera.h"
#include "model.h"
#include "normalmapshader.h"
#include "input.h"
#include "light.h"

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
  bool Frame();

private:
  bool Render(float, float, float);

private:
  D3D* m_Direct3D;
  Camera* m_Camera;
  NormalMapShader* m_NormalMapShader;
  Model* m_Model;
  Light* m_Light;
};

#endif
