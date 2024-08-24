#ifndef _POSITION_H_
#define _POSITION_H_

#include <math.h>

class Position
{
public:
	Position();
	Position(const Position&);
	~Position();

	void SetFrameTime(float);
	void GetRotation(float&);

	void TurnLeft(bool);
	void TurnRight(bool);

private:
	float m_frameTime;
	float m_rotationY;
	float m_leftTurnSpeed;
	float m_rightTurnSpeed;
};

#endif