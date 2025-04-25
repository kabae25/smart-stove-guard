#ifndef __EVENT_TIMER_H
#define __EVENT_TIMER_H

#include <Arduino.h>

class EventTimer
{
  enum TIMER_STATE : uint8_t {TIMER_IDLE, TIMER_RUNNING};

  protected:
    TIMER_STATE timerState = TIMER_IDLE;
    uint32_t startTime = 0;
    uint32_t duration = 0;
    
  public:
    EventTimer(void) {}
    EventTimer(unsigned long dur);
    
    bool checkExpired(bool autoRestart = false);
    bool CheckExpired(bool autoRestart = false) {return checkExpired(autoRestart);}

    unsigned long start(unsigned long dur);
    unsigned long Start(unsigned long dur) {return start(dur);}
    
    unsigned long restart(unsigned long dur = 0);
    unsigned long Restart(unsigned long dur = 0) {return restart(dur);}
    
    void cancel(void);
    void Cancel(void) {cancel();}

    bool isRunning(void);
    bool IsRunning(void) {return isRunning();}
};

#endif
