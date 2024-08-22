#ifndef _TIMER_H_
#define _TIMER_H_

#include <Windows.h>

class Timer
{
 public:
  Timer();
  Timer(const Timer&);
  ~Timer();

  bool Initialize();
  void Frame();

  float GetTime();

 private:
  float m_frequency;
  INT64 m_startTime;
  float m_frameTime;
};

#endif
