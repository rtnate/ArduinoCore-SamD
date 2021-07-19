#ifndef _MC_TIMING_TEMPO_H_
#define _MC_TIMING_TEMPO_H_

#include "MCTimingFunctions.h"

namespace MCTiming 
{
    class TempoPrescaler
    {
        public:
            TempoPrescaler(uint8_t value = 0): bitshift(value & 0x1F){};
            uint32_t value() const { return 1 << bitshift; };
            uint8_t shift() const { return bitshift; };
            uint32_t mod() const { return (1 << bitshift) - 1; };
            
            TempoPrescaler& operator=(const TempoPrescaler& other)
            {
                bitshift = other.bitshift;
                return *this;
            }

            TempoPrescaler& operator=(uint8_t value)
            {
                bitshift = value & 0x1F;
                return *this;
            }

            friend uint32_t operator%(uint32_t lhs, const TempoPrescaler& rhs)
            {
                return lhs & rhs.mod();
            }
        protected:
            uint8_t bitshift;

    };

    class Tempo 
    {
        public:
            static Tempo fromBPM(float bpm){ return Tempo(BPMToCPUTicks(bpm)); };
            static Tempo fromMilliseconds(float ms){ return Tempo(MillisecondsToCPUTicks(ms)); };
            Tempo(): ticks(BPMToCPUTicks(120)){};
            Tempo(uint32_t cpuTicks): ticks(cpuTicks){};
            Tempo(const Tempo& other): ticks(other.ticks){};
            void setBPM(float bpm){ ticks = BPMToCPUTicks(bpm); };
            void setMilliseconds(float ms){ ticks = MillisecondsToCPUTicks(ms); };
            void setTicks(uint32_t cpuTicks){ ticks = cpuTicks; };
            float getBPM() const { return CPUTicksToBPM(ticks); };
            float getMilliseconds() const { return CPUTicksToMilliseconds(ticks); };
            uint32_t getTicks() const { return ticks; };
            float getMidiClockMicroseconds() const 
            {
                return CPUTicksToMilliseconds(ticks) * 1000.f / 24.f;
            }
            TempoPrescaler getPrescaler() const { return TempoPrescaler(CalculateTempoPrescaler(ticks)); };
            uint16_t getTimerPeriod() const { return CalculateTempoTimerPeriod(ticks); };
            float getBeatFraction() const { return CalculateTempoTimerBeatFraction(ticks); };
            explicit operator uint32_t() const { return ticks; };
        protected:
            uint32_t ticks; //!< Tempo Stored As Number Of CPU Ticks
    };

    // enum TempoMultiplier
    // {
    //     TempoDiv16 = -16,
    //     TempoDiv8 = -8,
    //     TempoDiv4 = -4,
    //     TempoDiv2 = -2,
    //     TempoMult1 = 1,
    //     TempoMult2 = 2,
    //     TempoMult3 = 3,
    //     TempoMult4 = 4
    // };
}

#endif