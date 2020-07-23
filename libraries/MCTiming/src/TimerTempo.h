#ifndef _MC_TIMING_TIMER_TEMPO_H_
#define _MC_TIMING_TIMER_TEMPO_H_

#include "Tempo.h"

namespace MCTiming 
{
    class TimerTempoListener;

    class TimerTempo
    {
        public:
            TimerTempo(): tTempo(Tempo::fromBPM(120)), tPrescaler(), tPeriod(), listener(nullptr)
            {
                tPrescaler = tTempo.getPrescaler();
                tPeriod = tTempo.getTimerPeriod();
            }
            TimerTempo(const Tempo& tempo): tTempo(tempo), tPrescaler(tempo.getPrescaler()), 
                                            tPeriod(tempo.getTimerPeriod()), listener(nullptr){};
            void attachListener(TimerTempoListener& newListener){ listener = &newListener; };
            void dettachListener(){ listener = nullptr; };
            void update(Tempo newTempo);
            Tempo tempo() const { return tTempo; };
            TempoPrescaler prescaler() const { return tPrescaler; };
            uint16_t period() const { return tPeriod; };
        protected:
            TimerTempoListener* listener;
            Tempo tTempo;
            TempoPrescaler tPrescaler;
            uint16_t tPeriod;

    };

    class TimerTempoListener
    {
        public:
            virtual void newTempo(const TimerTempo& newTempo) = 0;
    };
}
#endif