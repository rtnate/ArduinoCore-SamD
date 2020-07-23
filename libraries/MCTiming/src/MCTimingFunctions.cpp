#include "MCTimingFunctions.h"

uint8_t MCTiming::CalculateTempoPrescaler(uint32_t tempo_cpu_ticks)
{
    uint32_t prescaler = 0;
    uint32_t timer_ticks = tempo_cpu_ticks / (TempoTimerCPUPrescaler * MidiClocksPerBeat);
    uint32_t last_ticks = timer_ticks;
    while (timer_ticks > TempoTimerTargetTickRate)
    {
        prescaler++;
        last_ticks = timer_ticks;
        timer_ticks = timer_ticks >> 1;
    }
    uint32_t valA = last_ticks - TempoTimerTargetTickRate;
    uint32_t valB = TempoTimerTargetTickRate - timer_ticks;
    if (prescaler <= 1) return 0;
    if (valA < valB || timer_ticks < TempoTimerMinimumTickRate) return prescaler - 1;
    return prescaler;
}

uint16_t MCTiming::CalculateTempoTimerPeriod(uint32_t tempo_cpu_ticks)
{
    constexpr uint32_t Div = TempoTimerCPUPrescaler * 24;
    uint32_t base_period = tempo_cpu_ticks / Div;
    uint8_t prescaler = CalculateTempoPrescaler(tempo_cpu_ticks);
    uint32_t period = base_period >> prescaler;
    return (uint16_t)period;
}

float MCTiming::CalculateTempoTimerBeatFraction(uint32_t tempo_cpu_ticks)
{
    uint8_t prescaler = CalculateTempoPrescaler(tempo_cpu_ticks);
    uint32_t divisor = 1 << prescaler;
    return 1.f / ((float) divisor * MidiClocksPerBeat);
}