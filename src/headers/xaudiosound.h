#ifndef _XAUDIOSOUND_H_
#define _XAUDIOSOUND_H_

#include "xaudio.h"

class XAudioSound
{
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
	XAudioSound();
	XAudioSound(const XAudioSound&);
	~XAudioSound();

	bool LoadTrack(IXAudio2*, char*, float);
	void ReleaseTrack();

	bool PlayTrack();
	bool StopTrack();

private:
	bool LoadStereoWaveFile(IXAudio2*, char*, float);
	void ReleaseWaveFile();

private:
	unsigned char* m_waveData;
	XAUDIO2_BUFFER m_audioBuffer;
	IXAudio2SourceVoice* m_sourceVoice;
};

#endif