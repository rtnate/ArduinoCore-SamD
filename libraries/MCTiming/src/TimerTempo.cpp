#include "TimerTempo.h"

using namespace MCTiming;

void TimerTempo::update(Tempo newTempo)
{
    tTempo = newTempo;
    tPrescaler = tTempo.getPrescaler();
    tPeriod = tTempo.getTimerPeriod();
    if (listener != nullptr) listener->newTempo(*this);
}