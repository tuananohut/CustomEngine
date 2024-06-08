#include "headers/system.h"

int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance, 
				   PSTR pScmdline, 
				   int iCmdshow) 
{
	System* mSystem;
	bool result;

	mSystem = new System;

	result = mSystem->Initialize();
	if(result)
	{
		mSystem->Run();
	}

	mSystem->Shutdown();
	delete mSystem;
	mSystem = nullptr;
}