#ifndef _MC_TIMING_FUNCTIONS_H_
#define _MC_TIMING_FUNCTIONS_H_

#include <Arduino.h>
#include <stdint.h>

namespace MCTiming 
{
    /**
     * @brief The CPU Frequency in Hertz (1/s)
     */
    constexpr uint32_t CPUFrequency = F_CPU;

    /**
     * @brief Preferred Tempo Timer Prescaler Division Factor.
     */
    constexpr uint32_t TempoTimerCPUPrescaler = F_CPU / 12000000ul;

    /**
     * @brief Frequency of the Tempo Timer/Counter
     */
    constexpr uint32_t TempoTimerFrequency = CPUFrequency / TempoTimerCPUPrescaler;

    /**
     * @brief Number of Midi Clock ticks in a single beat
     */
    constexpr uint32_t MidiClocksPerBeat = 24;

    /**
     * @brief Period of a Tempo Timer/Counter tick in milliseconds
     */
    constexpr float TempoTimerPeriodMs = (1000.f/ (float)TempoTimerFrequency);

    /**
     * @brief The desired Tempo Timer/Counter interrupt Frequency (~ 1 millisecond)
     */
    constexpr uint32_t TempoTimerTargetTickRate = 1.f / TempoTimerPeriodMs;

    /**
     * @brief The mininum Tempo Timer/Counter interrupt Frequency ( 0.8 milliseconds )
     */
    constexpr uint32_t TempoTimerMinimumTickRate = 0.8f / TempoTimerPeriodMs;

    /**
     * @brief Calculate the Tempo Timer/Counter prescaler bitshift
     * 
     * @param tempo_cpu_ticks The tempo in CPU Ticks
     * @return uint8_t The Tempo Timer/Counter prescaler bitshift 
     */
    uint8_t CalculateTempoPrescaler(uint32_t tempo_cpu_ticks);

    /**
     * @brief Calculate the Tempo Timer/Counter Period Register 
     * 
     * @param tempo_cpu_ticks The tempo in CPU Ticks
     * @return uint16_t The Tempo Timer/Counte Period
     */
    uint16_t CalculateTempoTimerPeriod(uint32_t tempo_cpu_ticks);

    /**
     * @brief Calculate the fraction of a beat that occurs per Tempo Timer/Counter interrupt
     * 
     * @param tempo_cpu_ticks The tempo in CPU Ticks
     * @return float The fraction of a beat per interrupt
     */
    float CalculateTempoTimerBeatFraction(uint32_t tempo_cpu_ticks);

    /**
     * @brief Get the amount of CPU Ticks (cycles) in the supplied milliseconds
     * 
     * @param ms The period in milliseconds
     * @return constexpr uint32_t The number of CPU Ticks (cycles)
     */
    constexpr inline uint32_t MillisecondsToCPUTicks(float ms)
    {
        //~~ C++ 14 Form would look like this ~~
        //constexpr float Mult = (float)CPUFrequency / 1000.f;
        //return (uint32_t)(Mult * ms);
        //~~~ Single Line Expression for C++11 Compatability ~~~ 
        return (uint32_t)( ((float)CPUFrequency / 1000.f) * ms);
    }

    /**
     * @brief Get the amount of CPU Ticks (cycles) in the supplied milliseconds
     * 
     * @param ms The period in milliseconds
     * @return constexpr uint32_t The number of CPU Ticks (cycles)
     */
    constexpr inline uint32_t MillisecondsToCPUTicks(uint32_t ms)
    {
        //~~ C++ 14 Form would look like this ~~
        //constexpr uint32_t Mult = CPUFrequency / 1000;
        //return Mult * ms;
        //~~~ Single Line Expression for C++11 Compatability ~~~ 
        return (CPUFrequency / 1000) * ms;
    }

    /**
     * @brief Get the tempo in Beats Per Minute from the supplied time in millieconds
     * 
     * @param ms The period in milliseconds
     * @return constexpr float The tempo in Beats Per Minute
     */
    constexpr inline float MillisecondsToBPM(float ms)
    {
        return 60000.f / ms;
    }

    /**
     * @brief Get the tempo in Beats Per Minute from the supplied time in microseconds
     * 
     * @param ms The period in microseconds
     * @return constexpr float The tempo in Beats Per Minute
     */
    constexpr inline float MicrosecondsToBPM(float us)
    {
        return 60000000.f / us;
    }

    /**
     * @brief Get the number of CPU Ticks (cycles) from the supplied tempo in BPM
     * 
     * @param bpm The tempo in Beats Per Minute
     * @return constexpr uint32_t The number of CPU Ticks (cycles)
     */
    constexpr inline uint32_t BPMToCPUTicks(float bpm)
    {
        //~~ C++ 14 Form would look like this ~~
        //constexpr float Mult = (1000.f)/(float) CPUFrequency;
        //constexpr float Div = 60000.f / Mult;
        //return Div / bpm
        //~~~ Single Line Expression for C++11 Compatability ~~~ 
        return (uint32_t)( ( (60000.f) / ( (1000.f) /(float)CPUFrequency ) ) / bpm );
    }

    /**
     * @brief Get the period in milliseconds from the supplied tempo in BPM
     * 
     * @param bpm The tempo in Beats Per Minute
     * @return constexpr float The period in milliseconds
     */
    constexpr inline float BPMToMilliseconds(float bpm)
    {
        return 60000.f / bpm;
    }

    /**
     * @brief Get the number of milliseconds in the supplied no of CPU Ticks (cycles)
     * 
     * @param ticks The number of CPU Ticks (cycles)
     * @return constexpr float The period in milliseconds
     */
    constexpr inline float CPUTicksToMilliseconds(uint32_t ticks)
    {
        //~~ C++ 14 Form would look like this ~~
        //constexpr float Mult = (1000.f)/(float) CPUFrequency;
        //return (float)ticks * Mult;
        //~~~ Single Line Expression for C++11 Compatability ~~~ 
        return (float) ticks * ( (1000.f)/(float) CPUFrequency );
    }

    /**
     * @brief Get the tempo in BPM from the the supplied period in CPU Ticks (cycles)
     * 
     * @param ticks The number of CPU Ticks (cycles)
     * @return constexpr float The tempo in Beats Per Minute
     */
    constexpr inline float CPUTicksToBPM(uint32_t ticks)
    {
        //~~ C++ 14 Form would look like this ~~
        //constexpr float Div = (60.f) * (float)CPUFrequency;
        //return Div / ticks;
        //~~~ Single Line Expression for C++11 Compatability ~~~ 
        return ( (60.f) * (float)CPUFrequency ) / ticks;
    }

    extern uint32_t getCPUTimestamp();
}
#endif