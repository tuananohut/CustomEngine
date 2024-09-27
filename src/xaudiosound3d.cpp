#include "headers/xaudiosound3d.h"

XAudioSound3D::XAudioSound3D()
{
	m_waveData = nullptr;
	m_sourceVoice = nullptr;
}

XAudioSound3D::XAudioSound3D(const XAudioSound3D& other) {}

XAudioSound3D::~XAudioSound3D() {}

bool XAudioSound3D::LoadTrack(IXAudio2* XAudio2, char* filename, float volume)
{
	bool result;

	InitializeEmitter();

	result = LoadMonoWaveFile(XAudio2, filename, volume);
	if (!result)
	{
		return false;
	}

	return true;
}

void XAudioSound3D::ReleaseTrack()
{
	if (m_sourceVoice)
	{
		m_sourceVoice->DestroyVoice();
		m_sourceVoice = nullptr;
	}

	ReleaseWaveFile();
}

bool XAudioSound3D::PlayTrack()
{
	HRESULT result;

	result = m_sourceVoice->Start(0, XAUDIO2_COMMIT_NOW);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool XAudioSound3D::StopTrack()
{
	HRESULT result;

	result = m_sourceVoice->Stop(0);
	if (FAILED(result))
	{
		return false;
	}

	result = m_sourceVoice->FlushSourceBuffers();
	if (FAILED(result))
	{
		return false;
	}

	result = m_sourceVoice->SubmitSourceBuffer(&m_audioBuffer);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}

void XAudioSound3D::Update3DPosition(float x, float y, float z)
{
	m_emitter.Position.x = x;
	m_emitter.Position.y = y;
	m_emitter.Position.z = z;
}

IXAudio2SourceVoice* XAudioSound3D::GetSourceVoice()
{
	return m_sourceVoice;
}

X3DAUDIO_EMITTER XAudioSound3D::GetEmitter()
{
	return m_emitter;
}

void XAudioSound3D::InitializeEmitter()
{
	ZeroMemory(&m_emitter, sizeof(&m_emitter));

	m_emitter.ChannelCount = 1;
	m_emitter.CurveDistanceScaler = 1.f;
	m_emitter.DopplerScaler = 1.f;

	m_emitter.Position.x = 0.f;
	m_emitter.Position.y = 0.f;
	m_emitter.Position.z = 0.f;
}

bool XAudioSound3D::LoadMonoWaveFile(IXAudio2* xAudio2, char* filename, float volume)
{
	FILE* filePtr;
	RiffWaveHeaderType riffWaveFileHeader;
	SubChunkHeaderType subChunkHeader;
	FmtType fmtData;
	WAVEFORMATEX waveFormat;
	int error;
	unsigned long long count;
	long seekSize;
	unsigned long dataSize;
	bool foundFormat, foundData;
	HRESULT result;

	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	count = fread(&riffWaveFileHeader, sizeof(riffWaveFileHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	if ((riffWaveFileHeader.chunkId[0] != 'R') || (riffWaveFileHeader.chunkId[1] != 'I') || (riffWaveFileHeader.chunkId[2] != 'F') || (riffWaveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	if ((riffWaveFileHeader.format[0] != 'W') || (riffWaveFileHeader.format[1] != 'A') || (riffWaveFileHeader.format[2] != 'V') || (riffWaveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	foundFormat = false;
	while (foundFormat == false)
	{
		count = fread(&subChunkHeader, sizeof(subChunkHeader), 1, filePtr);
		if (count != 1)
		{
			return false;
		}

		if ((subChunkHeader.subChunkId[0] == 'f') && (subChunkHeader.subChunkId[1] == 'm') && (subChunkHeader.subChunkId[2] == 't') && (subChunkHeader.subChunkId[3] == ' '))
		{
			foundFormat = true;
		}

		else
		{
			fseek(filePtr, subChunkHeader.subChunkSize, SEEK_CUR);
		}
	}

	count = fread(&fmtData, sizeof(fmtData), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	if (fmtData.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	if (fmtData.numChannels != 1)
	{
		return false;
	}

	if (fmtData.sampleRate != 44100)
	{
		return false;
	}

	if (fmtData.bitsPerSample != 16)
	{
		return false;
	}

	seekSize = subChunkHeader.subChunkSize - 16;
	fseek(filePtr, seekSize, SEEK_CUR);

	foundData = false;
	while (foundData == false)
	{
		count = fread(&subChunkHeader, sizeof(subChunkHeader), 1, filePtr);
		if (count != 1)
		{
			return false;
		}

		if ((subChunkHeader.subChunkId[0] == 'd') && (subChunkHeader.subChunkId[1] == 'a') && (subChunkHeader.subChunkId[2] == 't') && (subChunkHeader.subChunkId[3] == 'a'))
		{
			foundData = true;
		}

		else
		{
			fseek(filePtr, subChunkHeader.subChunkSize, SEEK_CUR);
		}
	}

	dataSize = subChunkHeader.subChunkSize;

	m_waveData = new unsigned char[dataSize];

	count = fread(m_waveData, 1, dataSize, filePtr);
	if (count != dataSize)
	{
		return false;
	}

	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = fmtData.sampleRate;
	waveFormat.wBitsPerSample = fmtData.bitsPerSample;
	waveFormat.nChannels = fmtData.numChannels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	m_audioBuffer.AudioBytes = dataSize;
	m_audioBuffer.pAudioData = m_waveData;
	m_audioBuffer.Flags = XAUDIO2_END_OF_STREAM;
	m_audioBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	m_audioBuffer.PlayBegin = 0;
	m_audioBuffer.PlayLength = 0;
	m_audioBuffer.LoopBegin = 0;
	m_audioBuffer.LoopLength = 0;
	m_audioBuffer.pContext = NULL;

	result = xAudio2->CreateSourceVoice(&m_sourceVoice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = m_sourceVoice->SubmitSourceBuffer(&m_audioBuffer);
	if (FAILED(result))
	{
		return false;
	}

	m_sourceVoice->SetVolume(volume);

	return true;
}

void XAudioSound3D::ReleaseWaveFile()
{
	if (m_waveData)
	{
		delete[] m_waveData;
		m_waveData = nullptr;
	}
}