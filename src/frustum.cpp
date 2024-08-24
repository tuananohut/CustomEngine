#include "headers/frustum.h"

Frustum::Frustum() {}

Frustum::Frustum(const Frustum& other) {}

Frustum::~Frustum() {}

void Frustum::ConstructFrustum(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float screenDepth)
{
	XMMATRIX finalMatrix;
	XMFLOAT4X4 projMatrix, matrix;
	float zMinimum, r, t;

	XMStoreFloat4x4(&projMatrix, projectionMatrix);

	zMinimum = -projMatrix._43 / projMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	projMatrix._33 = r;
	projMatrix._43 = -r * zMinimum;

	projectionMatrix = XMLoadFloat4x4(&projMatrix);

	finalMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

	XMStoreFloat4x4(&matrix, finalMatrix);

	m_planes[0].x = matrix._13;
	m_planes[0].y = matrix._23;
	m_planes[0].z = matrix._33;
	m_planes[0].w = matrix._43;

	t = (float)sqrt((m_planes[0].x * m_planes[0].x) + (m_planes[0].y * m_planes[0].y) + (m_planes[0].z * m_planes[0].z));
	
	m_planes[0].x /= t;
	m_planes[0].y /= t;
	m_planes[0].z /= t;
	m_planes[0].w /= t;

	m_planes[1].x = matrix._14 - matrix._13;
	m_planes[1].y = matrix._24 - matrix._23;
	m_planes[1].z = matrix._34 - matrix._33;
	m_planes[1].w = matrix._44 - matrix._43;

	t = (float)sqrt((m_planes[1].x * m_planes[1].x) + (m_planes[1].y * m_planes[1].y) + (m_planes[1].z * m_planes[1].z));

	m_planes[1].x /= t;
	m_planes[1].y /= t;
	m_planes[1].z /= t;
	m_planes[1].w /= t;

	m_planes[2].x = matrix._14 + matrix._11;
	m_planes[2].y = matrix._24 + matrix._21;
	m_planes[2].z = matrix._34 + matrix._31;
	m_planes[2].w = matrix._44 + matrix._41;

	t = (float)sqrt((m_planes[2].x * m_planes[2].x) + (m_planes[2].y * m_planes[2].y) + (m_planes[2].z * m_planes[2].z));

	m_planes[2].x /= t;
	m_planes[2].y /= t;
	m_planes[2].z /= t;
	m_planes[2].w /= t;

	m_planes[3].x = matrix._14 - matrix._11;
	m_planes[3].y = matrix._24 - matrix._21;
	m_planes[3].z = matrix._34 - matrix._31;
	m_planes[3].w = matrix._44 - matrix._41;

	t = (float)sqrt((m_planes[3].x * m_planes[3].x) + (m_planes[3].y * m_planes[3].y) + (m_planes[3].z * m_planes[3].z));

	m_planes[3].x /= t;
	m_planes[3].y /= t;
	m_planes[3].z /= t;
	m_planes[3].w /= t;

	m_planes[4].x = matrix._14 - matrix._12;
	m_planes[4].y = matrix._24 - matrix._22;
	m_planes[4].z = matrix._34 - matrix._32;
	m_planes[4].w = matrix._44 - matrix._42;

	t = (float)sqrt((m_planes[4].x * m_planes[4].x) + (m_planes[4].y * m_planes[4].y) + (m_planes[4].z * m_planes[4].z));

	m_planes[4].x /= t;
	m_planes[4].y /= t;
	m_planes[4].z /= t;
	m_planes[4].w /= t;

	m_planes[5].x = matrix._14 + matrix._12;
	m_planes[5].y = matrix._24 + matrix._22;
	m_planes[5].z = matrix._34 + matrix._32;
	m_planes[5].w = matrix._44 + matrix._42;

	t = (float)sqrt((m_planes[5].x * m_planes[5].x) + (m_planes[5].y * m_planes[5].y) + (m_planes[5].z * m_planes[5].z));

	m_planes[5].x /= t;
	m_planes[5].y /= t;
	m_planes[5].z /= t;
	m_planes[5].w /= t;
}

bool Frustum::CheckPoint(float x, float y, float z)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		if (((m_planes[i].x * x) + (m_planes[i].y * y) + (m_planes[i].z * z) + m_planes[i].w) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		if (m_planes[i].x * (xCenter - radius) + 
			m_planes[i].y * (yCenter - radius) + 
			m_planes[i].z * (zCenter - radius) + 
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter + radius) +
			m_planes[i].y * (yCenter - radius) +
			m_planes[i].z * (zCenter - radius) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter - radius) +
			m_planes[i].y * (yCenter + radius) +
			m_planes[i].z * (zCenter - radius) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter + radius) +
			m_planes[i].y * (yCenter + radius) +
			m_planes[i].z * (zCenter - radius) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter - radius) +
			m_planes[i].y * (yCenter - radius) +
			m_planes[i].z * (zCenter + radius) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter + radius) +
			m_planes[i].y * (yCenter - radius) +
			m_planes[i].z * (zCenter + radius) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter - radius) +
			m_planes[i].y * (yCenter + radius) +
			m_planes[i].z * (zCenter + radius) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter + radius) +
			m_planes[i].y * (yCenter + radius) +
			m_planes[i].z * (zCenter + radius) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool Frustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		if ((m_planes[i].x * xCenter) + (m_planes[i].y * yCenter) + (m_planes[i].z * zCenter) + m_planes[i].w < -radius)
		{
			return false;
		}
	}

	return true;
}

bool Frustum::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		if (m_planes[i].x * (xCenter - xSize) +
			m_planes[i].y * (yCenter - ySize) +
			m_planes[i].z * (zCenter - zSize) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter + xSize) +
			m_planes[i].y * (yCenter - ySize) +
			m_planes[i].z * (zCenter - zSize) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter - xSize) +
			m_planes[i].y * (yCenter + ySize) +
			m_planes[i].z * (zCenter - zSize) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter + xSize) +
			m_planes[i].y * (yCenter + ySize) +
			m_planes[i].z * (zCenter - zSize) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter - xSize) +
			m_planes[i].y * (yCenter - ySize) +
			m_planes[i].z * (zCenter + zSize) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter + xSize) +
			m_planes[i].y * (yCenter - ySize) +
			m_planes[i].z * (zCenter + zSize) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter - xSize) +
			m_planes[i].y * (yCenter + ySize) +
			m_planes[i].z * (zCenter + zSize) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		if (m_planes[i].x * (xCenter + xSize) +
			m_planes[i].y * (yCenter + ySize) +
			m_planes[i].z * (zCenter + zSize) +
			m_planes[i].w >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}