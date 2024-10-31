#include "headers/viewpoint.h"

ViewPoint::ViewPoint() {}

ViewPoint::ViewPoint(const ViewPoint& other) {}

ViewPoint::~ViewPoint() {}

void ViewPoint::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}

void ViewPoint::SetLookAt(float x, float y, float z)
{
	m_lookAt = XMFLOAT3(x, y, z);
}

void ViewPoint::SetProjectionParameters(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	m_fieldOfView = fieldOfView;
	m_aspectRatio = aspectRatio;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
}

void ViewPoint::GenerateViewMatrix()
{
	XMFLOAT3 up;
	XMVECTOR upVector, positionVector, lookAtVector;

	up.x = 0.f;
	up.y = 1.f;
	up.z = 0.f;

	upVector = XMLoadFloat3(&up);
	positionVector = XMLoadFloat3(&m_position);
	lookAtVector = XMLoadFloat3(&m_lookAt);

	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void ViewPoint::GenerateProjectionMatrix()
{
	m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
}

void ViewPoint::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void ViewPoint::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}