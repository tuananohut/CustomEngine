#include "headers/xinput.h"

XInput::XInput() {}

XInput::XInput(const XInput& other) {}

XInput::~XInput() {}

bool XInput::Initialize()
{
    int i;

    for (i = 0; i < 4; i++)
    {
        m_controllerActive[i] = false;
    }

    return true;
}

void XInput::Frame()
{
    unsigned long i, result;

    for (i = 0; i < 4; i++)
    {
        ZeroMemory(&m_controllerState[i], sizeof(XINPUT_STATE));

        result = XInputGetState(i, &m_controllerState[i]);

        if (result == ERROR_SUCCESS)
        {
            m_controllerActive[i] = true;
        }
        else
        {
            m_controllerActive[i] = false;
        }
    }
}

bool XInput::IsControllerActive(int index)
{
    if ((index < 0) || (index > 3))
    {
        return false;
    }

    return m_controllerActive[index];
}

bool XInput::IsButtonADown(int index)
{
    if (m_controllerState[index].Gamepad.wButtons & XINPUT_GAMEPAD_A)
    {
        return true;
    }

    return false;
}

bool XInput::IsButtonBDown(int index)
{
    if (m_controllerState[index].Gamepad.wButtons & XINPUT_GAMEPAD_B)
    {
        return true;
    }

    return false;
}

float XInput::GetLeftTrigger(int index)
{
    unsigned char triggerValue;
    float finalValue;

    triggerValue = m_controllerState[index].Gamepad.bLeftTrigger;

    if (triggerValue < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
    {
        return 0.f;
    }

    finalValue = (float)triggerValue / 255.f;

    return finalValue;
}

float XInput::GetRightTrigger(int index)
{
    unsigned char triggerValue;
    float finalValue;

    triggerValue = m_controllerState[index].Gamepad.bRightTrigger;

    if (triggerValue < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
    {
        return 0.f;
    }

    finalValue = (float)triggerValue / 255.f;

    return finalValue;
}

void XInput::GetLeftThumbStickLocation(int index, int& thumbLeftX, int& thumbLeftY)
{
    thumbLeftX = (int)m_controllerState[index].Gamepad.sThumbLX;
    thumbLeftY = (int)m_controllerState[index].Gamepad.sThumbLY;

    if (IsLeftThumbStickInDeadZone(index) == true)
    {
        thumbLeftX = 0;
        thumbLeftY = 0;
    }
}

bool XInput::IsLeftThumbStickInDeadZone(int index)
{
    int thumbLeftX, thumbLeftY, magnitude;

    thumbLeftX = (int)m_controllerState[index].Gamepad.sThumbLX;
    thumbLeftY = (int)m_controllerState[index].Gamepad.sThumbLY;

    magnitude = (int)sqrt((thumbLeftX * thumbLeftX) + (thumbLeftY * thumbLeftY));

    if (magnitude < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
    {
        return true;
    }

    return false;
}