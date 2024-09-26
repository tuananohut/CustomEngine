#ifndef _XAUDIO_H_
#define _XAUDIO_H_

#pragma comment(lib, "xaudio2.lib")

#include <xaudio2.h>
#include <stdio.h>

class XAudio
{
public:
	XAudio();
	XAudio(const XAudio&);
	~XAudio();
	
	bool Initialize();
	bool Shutdown();

	IXAudio2* GetXAudio2();

private:
	IXAudio2* m_xAudio2;
	IXAudio2MasteringVoice* m_masterVoice;
};


#endif
