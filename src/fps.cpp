#include "headers/fps.h"

Fps::Fps() {}

Fps::Fps(const Fps& other) {}

Fps::~Fps() {}

void Fps::Initialize()
{
	m_fps = 0;
	m_count = 0;

	m_startTime = timeGetTime();
}

void Fps::Frame()
{
	m_count++;

	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}
}


int Fps::GetFps()
{
	return m_fps;
}
