#pragma once

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <DirectXMath.h>

using namespace DirectX;

class Light
{
public:
	Light();
	Light(const Light&);
	~Light();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);
	void SetPosition(float, float, float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetDirection();
	XMFLOAT4 GetSpecularColor();
	float GetSpecularPower();
	XMFLOAT3 GetPosition();

private:
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
	XMFLOAT4 m_specularColor;
	float m_specularPower;
	XMFLOAT3 m_position;
};

#endif
