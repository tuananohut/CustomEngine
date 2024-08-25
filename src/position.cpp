#include "headers/position.h"

Position::Position()
{
	m_frameTime = 0.f;
	m_rotationY = 0.f;
	m_leftTurnSpeed = 0.f;
	m_rightTurnSpeed = 0.f;
}

Position::Position(const Position& other) {}

Position::~Position() {}

void Position::SetFrameTime(float time)
{
	m_frameTime = time;
}

void Position::GetRotation(float& y)
{
	y = m_rotationY;
}

void Position::TurnLeft(bool keydown)
{
	if (keydown)
	{
		m_leftTurnSpeed += m_frameTime * 1.5f;

		if (m_leftTurnSpeed > (m_frameTime * 200.f))
		{
			m_leftTurnSpeed = m_frameTime * 200.f;
		}
	}

	else
	{
		m_leftTurnSpeed -= m_frameTime * 1.f;

		if (m_leftTurnSpeed < 0.f)
		{
			m_leftTurnSpeed = 0.f;
		}
	}

	m_rotationY -= m_leftTurnSpeed;
	if (m_rotationY < 0.f)
	{
		m_rotationY += 360.f;
	}
}

void Position::TurnRight(bool keydown)
{
	if (keydown)
	{
		m_rightTurnSpeed += m_frameTime * 1.5f;

		if (m_rightTurnSpeed > (m_frameTime * 200.f))
		{
			m_rightTurnSpeed = m_frameTime * 200.f;
		}
	}

	else
	{
		m_rightTurnSpeed -= m_frameTime * 1.f;

		if (m_rightTurnSpeed < 0.f)
		{
			m_rightTurnSpeed = 0.f;
		}
	}

	m_rotationY += m_rightTurnSpeed;
	if (m_rotationY > 360.f)
	{
		m_rotationY -= 360.f;
	}
}