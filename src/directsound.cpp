#include "headers/directsound.h"

DirectSound::DirectSound()
{
	m_DirectSound = nullptr;
	m_primaryBuffer = nullptr;
	m_listener = nullptr;
}

DirectSound::DirectSound(const DirectSound& other) {}

DirectSound::~DirectSound() {}

bool DirectSound::Initialize(HWND hwnd) 
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
	{
		return false;
	}

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	result = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
	{
		return false;
	}

	result = m_primaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&m_listener);
	if (FAILED(result))
	{
		return false;
	}

	m_listener->SetPosition(0.f, 0.f, 0.f, DS3D_IMMEDIATE);

	return true;
}

void DirectSound::Shutdown()
{
	if (m_listener)
	{
		m_listener->Release();
		m_listener = nullptr;
	}

	if (m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = nullptr;
	}

	if (m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = nullptr;
	}
}

IDirectSound8* DirectSound::GetDirectSound()
{
	return m_DirectSound;
}