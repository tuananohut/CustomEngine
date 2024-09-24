#include "headers/sound.h"

Sound::Sound()
{
	m_secondaryBuffer = nullptr;
}


Sound::Sound(const Sound& other) {}

Sound::~Sound() {}

bool Sound::LoadTrack(IDirectSound8* DirectSound, char* filename, long volume)
{
	bool result;

	result = LoadStereoWaveFile(DirectSound, filename, volume);
	if (!result)
	{
		return false;
	}

	return true;
}

void Sound::ReleaseTrack()
{
	ReleaseWaveFile();
}

bool Sound::PlayTrack()
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

	return true;
}

bool Sound::StopTrack()
{
	HRESULT result;

	result = m_secondaryBuffer->Stop();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool Sound::LoadStereoWaveFile(IDirectSound8* DirectSound, char* filename, long volume)
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

	if ((riffWaveFileHeader.chunkId[0] != 'R') || (riffWaveFileHeader.chunkId[1] != 'I') || (riffWaveFileHeader.chunkId[2] != 'F'), (riffWaveFileHeader.chunkId[3] != 'F'))
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

		if((subChunkHeader.subChunkId[0] == 'f') && (subChunkHeader.subChunkId[1] == 'm') && (subChunkHeader.subChunkId[2] == 't') && (subChunkHeader.subChunkId[3] == ' '))
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

	if (fmtData.numChannels != 2)
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

	/* not completed */



}