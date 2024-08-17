#include "headers/input.h"

Input::Input()
{
	m_direcInput = nullptr;
	m_keyboard = nullptr;
	m_mouse = nullptr;
}

Input::Input(const Input& other)
{
	
}

Input::~Input()
{

}

void Input::Initialize(HINSTANCE hInstance, 
					   HWND hwnd,
					   int screenWidth,
					   int screenHeight)
{
	HRESULT result;
}


