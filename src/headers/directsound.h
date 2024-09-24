#ifndef _DIRECTSOUND_H_
#define _DIRECTSOUND_H_

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

class DirectSound
{
public:
	DirectSound();
	DirectSound(const DirectSound&);
	~DirectSound();

	bool Initialize(HWND);
	void Shutdown();

	IDirectSound8* GetDirectSound();

private:
	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;
	IDirectSound3DListener8* m_listener;
};

#endif