#include "headers/xaudio.h"

XAudio::XAudio()
{
	m_xAudio2 = nullptr;
	m_masterVoice = nullptr;
	m_matrixCoefficients = nullptr;
}

XAudio::XAudio(const XAudio& other) {}

XAudio::~XAudio() {}

bool XAudio::Initialize()
{
	HRESULT result;
	DWORD dwChannelMask;

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

	m_masterVoice->GetChannelMask(&dwChannelMask);

	result = X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, m_X3DInstance);
	if (FAILED(result))
	{
		return false;
	}

	ZeroMemory(&m_listener, sizeof(m_listener));

	m_listener.Position.x = 0.f;
	m_listener.Position.y = 0.f;
	m_listener.Position.z = 0.f;

	m_listener.OrientFront.x = 0.f;
	m_listener.OrientFront.y = 0.f;
	m_listener.OrientFront.z = 1.f;

	m_listener.OrientTop.x = 0.f;
	m_listener.OrientTop.y = 1.f;
	m_listener.OrientTop.z = 0.f;

	m_masterVoice->GetVoiceDetails(&m_deviceDetails);

	m_matrixCoefficients = new float[m_deviceDetails.InputChannels];

	ZeroMemory(&m_DSPSettings, sizeof(&m_DSPSettings));

	m_DSPSettings.SrcChannelCount = 1;
	m_DSPSettings.DstChannelCount = m_deviceDetails.InputChannels;
	m_DSPSettings.pMatrixCoefficients = m_matrixCoefficients;

	return true;
}

void XAudio::Shutdown()
{
	if (m_matrixCoefficients)
	{
		delete[] m_matrixCoefficients;
		m_matrixCoefficients = nullptr;
	}

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

bool XAudio::Frame(X3DAUDIO_EMITTER emitter, IXAudio2SourceVoice* sourceVoice)
{
	HRESULT result;

	X3DAudioCalculate(m_X3DInstance,
					  &m_listener,
					  &emitter,
					  X3DAUDIO_CALCULATE_MATRIX |
					  X3DAUDIO_CALCULATE_DOPPLER |
					  X3DAUDIO_CALCULATE_LPF_DIRECT |
					  X3DAUDIO_CALCULATE_REVERB,
					  &m_DSPSettings);

	result = sourceVoice->SetOutputMatrix(m_masterVoice, 1, m_deviceDetails.InputChannels, m_DSPSettings.pMatrixCoefficients);
	if (FAILED(result))
	{
		return false;
	}

	result = sourceVoice->SetFrequencyRatio(m_DSPSettings.DopplerFactor);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

IXAudio2* XAudio::GetXAudio2()
{
	return m_xAudio2;
}
