#ifndef _SOUND_H_
#define _SOUND_H_

#include "directsound.h"

class Sound
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
	Sound();
	Sound(const Sound&);
	~Sound();

	bool LoadTrack(IDirectSound8*, char*, long);
	void ReleaseTrack();

	bool PlayTrack();
	bool StopTrack();

private: 
	bool LoadStereoWaveFile(IDirectSound8*, char*, long);
	void ReleaseWaveFile();

private:
	IDirectSoundBuffer8* m_secondaryBuffer;
};

#endif