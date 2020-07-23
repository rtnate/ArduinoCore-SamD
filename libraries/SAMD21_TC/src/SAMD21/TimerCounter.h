#ifndef _SAMD21_TIMER_COUNTER_H_
#define _SAMD21_TIMER_COUNTER_H_

#include "./TimerCounterDefinitions.h"

namespace SAMD21 
{
    class TimerCounter 
    {
        public:
            typedef TimerCounterModuleInstances ModuleSelection;
            typedef TimerCounterPrescaler Prescaler;
            typedef TimerCounterWaveGenerationMode WaveGenerationMode;
            typedef TimerCounterMode CountMode; 
            static TimerCounter module(ModuleSelection moduleNo);
            static constexpr uint32_t InterruptPrioity = ((1<<__NVIC_PRIO_BITS) - 1);
            TimerCounter(const TimerCounter& other): tc(other.tc){};
            void reset();
            void begin(CountMode mode = Count16, Prescaler prescaler = DIV1, WaveGenerationMode waveMode = NormalFrequency, bool runInStandBy = false);
            void init(CountMode mode = Count16, Prescaler prescaler = DIV1, WaveGenerationMode waveMode = NormalFrequency, bool runInStandBy = false);
            void enableInterrupts(bool compareMatch0, bool compareMatch1, bool overflow, bool error = false, bool syncReady = false);
            void enable();
            void setCompareChannel0(uint16_t data);
            void setCompareChannel1(uint16_t data);
            void setPeriod(uint8_t data);
            void setCounter(uint16_t data);
            uint16_t getCount();
            uint32_t readInterruptFlags(){ return tc.COUNT16.INTFLAG.reg; };
            void clearInterruptFlags(uint32_t flags){ tc.COUNT16.INTFLAG.reg = flags; };
        protected:
            TimerCounter(Tc& tcModule): tc(tcModule){};
            Tc& tc;
            CountMode mode;
            bool isEnabled(){ return (bool)(tc.COUNT16.CTRLA.bit.ENABLE); };
            CountMode getMode(){ return static_cast<CountMode>(tc.COUNT16.CTRLA.bit.MODE); };
            void initClocks();
            void enableNVIC();
    };
}
#endif