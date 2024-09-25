#include "headers/sound3d.h"

Sound3D::Sound3D()
{
	m_secondaryBuffer = nullptr;
	m_secondary3DBuffer = nullptr;
}

Sound3D::Sound3D(const Sound3D& other) {}

Sound3D::~Sound3D() {}

bool Sound3D::LoadTrack(IDirectSound8* DirectSound, char* filename, long volume)
{
	bool result;

	result = LoadMonoWaveFile(DirectSound, filename, volume);
	if (!result)
	{
		return false;
	}

	return true;
}

void Sound3D::ReleaseTrack()
{
	ReleaseWaveFile();
}

bool Sound3D::PlayTrack()
{
	HRESULT result;

	result = m_secondaryBuffer->SetCurrentPosition(0);
	if (FAILED(result))
	{
		return false;
	}

	result = m_secondaryBuffer->Play(0, 0, 0);
	if (FAILED(result))
	{
		return false;
	}

	return false;
}

bool Sound3D::StopTrack()
{
	HRESULT result;

	result = m_secondaryBuffer->Stop();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool Sound3D::LoadMonoWaveFile(IDirectSound8* DirectSound, char* filename, long volume)
{
	FILE* filePtr;
	RiffWaveHeaderType riffWaveFileHeader;
	SubChunkHeaderType subChunkHeader;
	FmtType fmtData;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char *waveData, *bufferPtr;
	unsigned long long count;
	unsigned long dataSize, bufferSize;
	long seekSize;
	int error;
	bool foundFormat, foundData;

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
	while(foundFormat == false)
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
	while(foundData == false)
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

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = fmtData.sampleRate;
	waveFormat.wBitsPerSample = fmtData.bitsPerSample;
	waveFormat.nChannels = fmtData.numChannels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwBufferBytes = dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;

	result = DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{ 
		return false;
	}

	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	tempBuffer->Release();
	tempBuffer = nullptr;

	waveData = new unsigned char[dataSize];

	count = fread(waveData, 1, dataSize, filePtr);
	if (count != dataSize)
	{
		return false;
	}

	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	result = m_secondaryBuffer->Lock(0, dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		return false;
	}

	memcpy(bufferPtr, waveData, dataSize);

	result = m_secondaryBuffer->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
	{
		return false;
	}

	delete[] waveData;
	waveData = nullptr;

	result = m_secondaryBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&m_secondary3DBuffer);
	if (FAILED(result))
	{
		return false;
	}

	result = m_secondary3DBuffer->SetPosition(0.f, 0.f, 0.f, DS3D_IMMEDIATE);
	if (FAILED(result))
	{
		return false;
	}

	result = m_secondaryBuffer->SetVolume(volume);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Sound3D::ReleaseWaveFile()
{
	if (m_secondary3DBuffer)
	{
		m_secondary3DBuffer->Release();
		m_secondary3DBuffer = nullptr;
	}

	if (m_secondaryBuffer)
	{
		m_secondaryBuffer->Release();
		m_secondaryBuffer = nullptr;
	}
}

bool Sound3D::Update3DPosition(float posX, float posY, float posZ)
{
	HRESULT result;

	result = m_secondary3DBuffer->SetPosition(posX, posY, posZ, DS3D_IMMEDIATE);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}