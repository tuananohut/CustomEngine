#ifndef _SOUND3D_H_
#define _SOUND3D_H_

#include "directsound.h"

class Sound3D
{
private:
	struct RiffWaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
	};

	struct SubChunkHeaderType
	{
		char subChunkId[4];
		unsigned long subChunkSize;
	};

	struct FmtType
	{
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
	};

public:
	Sound3D();
	Sound3D(const Sound3D&);
	~Sound3D();

	bool LoadTrack(IDirectSound8*, char*, long);
	void ReleaseTrack();

	bool PlayTrack();
	bool StopTrack();
	bool Update3DPosition(float, float, float);

private:
	bool LoadMonoWaveFile(IDirectSound8*, char*, long);
	void ReleaseWaveFile();

private:
	IDirectSoundBuffer8* m_secondaryBuffer;
	IDirectSound3DBuffer8* m_secondary3DBuffer;
};

#endif