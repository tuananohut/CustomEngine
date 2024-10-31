#ifndef _VIEWPOINT_H_
#define _VIEWPOINT_H_

#include <DirectXMath.h>

using namespace DirectX;

class ViewPoint
{
public:
	ViewPoint();
	ViewPoint(const ViewPoint&);
	~ViewPoint();

	void SetPosition(float, float, float);
	void SetLookAt(float, float, float);
	void SetProjectionParameters(float, float, float, float);

	void GenerateViewMatrix();
	void GenerateProjectionMatrix();

	void GetViewMatrix(XMMATRIX&);
	void GetProjectionMatrix(XMMATRIX&);

private:
	XMMATRIX m_viewMatrix, m_projectionMatrix;
	XMFLOAT3 m_position, m_lookAt;
	float m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane;
};

#endif