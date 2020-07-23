#ifndef _MC_TIMING_TIMER_H_
#define _MC_TIMING_TIMER_H_

#include <SAMD21_TC.h>
#include "./TimerTempo.h"

namespace MCTiming 
{

    class Timer: public TimerTempoListener
    {
        public:
            using HardwareTimer = SAMD21::TimerCounter;
            Timer(HardwareTimer::ModuleSelection timerModule, TimerTempo& timerTempo): hwTimer(HardwareTimer::module(timerModule)), tempo(timerTempo)
            {
                tempo.attachListener(*this);
            };
            virtual void tick(){};
            void begin();
            void newTempo(const TimerTempo& tempo);
            static inline void ISRHandler(Timer& instance);
        protected:
            HardwareTimer hwTimer;
            TimerTempo& tempo;
    };

    void Timer::ISRHandler(Timer& instance)
    {
        uint32_t flags = instance.hwTimer.readInterruptFlags();
        if (flags & TC_INTFLAG_OVF)
        {
            instance.tick();
            instance.hwTimer.clearInterruptFlags(TC_INTFLAG_OVF);
        }
    }
}

#define CREATE_MCTIMING_TIMER_ISR(TIMER_INSTANCE) \
    MCTiming::Timer::ISRHandler(TIMER_INSTANCE);

#endif