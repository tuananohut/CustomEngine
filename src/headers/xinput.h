#ifndef _XINPUTCLASS_H_
#define _XINPUTCLASS_H_

#pragma comment(lib, "Xinput.lib")

#include <windows.h>
#include <xinput.h>
#include <math.h>

class XInput
{
public:
	XInput();
	XInput(const XInput&);
	~XInput();

	bool Initialize();
	void Shutdown();
	void Frame();

	bool IsControllerActive(int);

	bool IsButtonADown(int);
	bool IsButtonBDown(int);

	float GetLeftTrigger(int);
	float GetRightTrigger(int);

	void GetLeftThumbStickLocation(int, int&, int&);

private:
	bool IsLeftThumbStickInDeadZone(int);

private:
	XINPUT_STATE m_controllerState[4];
	bool m_controllerActive[4];
};

#endif