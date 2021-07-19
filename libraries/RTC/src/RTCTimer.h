#ifndef _RTC_TIMER_LIB_H_
#define _RTC_TIMER_LIB_H_

#include "RTCManager.h"
#include "RTCTimerValue.h"

class RTCTimer 
{
    public:
        static void start()
        {
            RealTimeClock.begin(RTCMode::RTICKS, true);
        }

        static RTCTimerValue currentTime()
        {
            return RTCTimerValue(RealTimeClock.getCounter());
        }
};

#endif
