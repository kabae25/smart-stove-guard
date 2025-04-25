#include <event_timer.h>

EventTimer::EventTimer(unsigned long dur)
{
  start(dur);
}

bool EventTimer::checkExpired(bool autoRestart)
{
    bool retVal = false;
    if(millis() - startTime > duration && timerState == TIMER_RUNNING)
    {
      if(autoRestart) restart();
      else timerState = TIMER_IDLE;
      retVal = true;
    }
    
    return retVal;
}

unsigned long EventTimer::start(unsigned long dur)
{
  startTime = millis();
  duration = dur;
  timerState = TIMER_RUNNING;
  
  return startTime + dur;
}

unsigned long EventTimer::restart(unsigned long dur) //need a better name?
{
  if(dur != 0) duration = dur;
  startTime += duration;
  timerState = TIMER_RUNNING;
  
  return startTime + duration;
}

void EventTimer::cancel(void)
{
  timerState = TIMER_IDLE;
}

bool EventTimer::isRunning(void)
{
    return timerState == TIMER_RUNNING;
}
