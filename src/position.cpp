#include "headers/position.h"

Position::Position()
{
	m_positionX = 0.f;
	m_positionY = 0.f;
	m_positionZ = 0.f;

	m_rotationX = 0.f;
	m_rotationY = 0.f;
	m_rotationZ = 0.f;

	m_frameTime = 0.f;
	
	m_leftSpeed = 0.f;
	m_rightSpeed = 0.f;
}

Position::Position(const Position& other) {}

Position::~Position() {}

void Position::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}

void Position::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

void Position::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
}

void Position::GetRotation(float& x, float& y, float& z)
{
	x = m_rotationX;
	y = m_rotationY;
	z = m_rotationZ;
}


void Position::SetFrameTime(float time)
{
	m_frameTime = time;
}

void Position::MoveLeft(bool keydown)
{
	float radians;

	if (keydown)
	{
		m_leftSpeed += m_frameTime * 0.25f;

		if (m_leftSpeed > (m_frameTime * 50.f))
		{
			m_leftSpeed = m_frameTime * 50.f;
		}
	}

	else
	{
		m_leftSpeed -= m_frameTime * 0.25f;

		if (m_leftSpeed < 0.f)
		{
			m_leftSpeed = 0.f;
		}
	}

	radians = m_rotationY * 0.0174532925f;

	m_positionX -= cosf(radians) * m_leftSpeed;
	m_positionZ -= sinf(radians) * m_leftSpeed;
}

void Position::MoveRight(bool keydown)
{
	float radians;

	if (keydown)
	{
		m_rightSpeed += m_frameTime * 0.25f;

		if (m_rightSpeed > (m_frameTime * 50.f))
		{
			m_rightSpeed = m_frameTime * 50.f;
		}
	}

	else
	{
		m_rightSpeed -= m_frameTime * 0.25f;

		if (m_rightSpeed < 0.f)
		{
			m_rightSpeed = 0.f;
		}
	}

	radians = m_rotationY * 0.0174532925f;

	m_positionX += cosf(radians) * m_rightSpeed;
	m_positionZ += sinf(radians) * m_rightSpeed;
}