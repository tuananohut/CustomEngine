#pragma once

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <Windows.h>
#include <cmath>

#include "d3d.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "clipplaneshader.h"
// #include "light.h"
// #include "lightshader.h"
// #include "modellist.h"
// #include "timer.h"
// #include "position.h"
// #include "colorshader.h"
// #include "shadermanager.h"

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
  bool Render(float, float);

private:
  D3D* m_Direct3D;
  Camera* m_Camera;
  Model* m_Model;
  ClipPlaneShader* m_ClipPlaneShader;
  // ShaderManager* m_ShaderManager;
  // Light* m_Light;
  // XMMATRIX m_baseViewMatrix;
  // LightShader* m_LightShader;
  // ModelList* m_ModelList;
  // Timer* m_Timer;
  // Position* m_Position;
  // ColorShader* m_ColorShader;

};

#endif
