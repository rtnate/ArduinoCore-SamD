#include "Timer.h"

using namespace MCTiming;
using namespace SAMD21;

void Timer::begin()
{
    Serial.println("Timer::begin() called.");
    hwTimer.init(Count16, DIV4, MatchFrequency, true);
    hwTimer.enable();
    tempo.update(Tempo::fromBPM(120));
    hwTimer.enableInterrupts(false, false, true);
}

void Timer::newTempo(const TimerTempo& tempo)
{
    uint16_t newTimerPeriod = tempo.period();
    uint32_t prescaler = tempo.prescaler().value();
    uint32_t milliseconds = tempo.tempo().getMilliseconds();
    Serial.println("New Tempo.");
    Serial.print("Tempo (ms): ");
    Serial.println(milliseconds);
    Serial.print("Tempo Prescaler: ");
    Serial.println(prescaler);
    Serial.print("Timer Period: ");
    Serial.println(newTimerPeriod);
    hwTimer.setCompareChannel0(newTimerPeriod);
}