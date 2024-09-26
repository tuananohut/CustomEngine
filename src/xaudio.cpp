#include "headers/xaudio.h"

XAudio::XAudio()
{
	m_xAudio2 = nullptr;
	m_masterVoice = nullptr;
}

XAudio::XAudio(const XAudio& other) {}

XAudio::~XAudio() {}

bool XAudio::Initialize()
{
	HRESULT result;

	result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(result))
	{
		return false;
	}

	result = XAudio2Create(&m_xAudio2, 0, XAUDIO2_USE_DEFAULT_PROCESSOR);
	if (FAILED(result))
	{
		return false;
	}

	result = m_xAudio2->CreateMasteringVoice(&m_masterVoice);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void XAudio::Shutdown()
{
	if (m_masterVoice)
	{
		m_masterVoice->DestroyVoice();
		m_masterVoice = nullptr;
	}

	if (m_xAudio2)
	{
		m_xAudio2->Release();
		m_xAudio2 = nullptr;
	}

	CoUninitialize();
}

IXAudio2* XAudio::GetXAudio2()
{
	return m_xAudio2;
}