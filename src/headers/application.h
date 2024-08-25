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
#include "lightshader.h"
#include "modellist.h"
#include "timer.h"
#include "position.h"
#include "frustum.h"
#include "shadermanager.h"

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
  bool Render(float);

private:
  D3D* m_Direct3D;
  Camera* m_Camera;
  ShaderManager* m_ShaderManager;
  Model* m_Model;
  Model* m_Model1;
  Model* m_Model2;
  Light* m_Light;
  LightShader* m_LightShader;
  ModelList* m_ModelList;
  Timer* m_Timer;
  Position* m_Position;
  Frustum* m_Frustum;
  XMMATRIX m_baseViewMatrix;
};

#endif
