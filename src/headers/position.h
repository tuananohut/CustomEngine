#ifndef _POSITION_H_
#define _POSITION_H_

#include <math.h>

class Position
{
public:
	Position();
	Position(const Position&);
	~Position();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void GetPosition(float&, float&, float&);
	void GetRotation(float&, float&, float&);

	void SetFrameTime(float);

	void MoveLeft(bool);
	void MoveRight(bool);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	float m_frameTime;

	float m_leftSpeed, m_rightSpeed;
};

#endif