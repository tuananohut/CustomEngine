#include "headers/timer.h"

Timer::Timer() {}

Timer::Timer(const Timer& other) {}

Timer::~Timer() {}

bool Timer::Initialize()
{
  INT64 frequency;

  QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
  if(frequency == 0)
    {
      return false;
    }

  m_frequency = (float)frequency;

  QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

  return true;
}

void Timer::Frame()
{
  INT64 currentTime;
  INT64 elapsedTicks;

  QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

  elapsedTicks = currentTime - m_startTime;

  m_frameTime = (float)elapsedTicks / m_frequency;

  m_startTime = currentTime;

  return;
}

float Timer::GetTime()
{
  return m_frameTime;
}
