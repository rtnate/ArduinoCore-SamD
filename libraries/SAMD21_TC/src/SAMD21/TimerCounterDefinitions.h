#ifndef _SAMD21_TIMER_COUNTER_DEFINITIONS_H_
#define _SAMD21_TIMER_COUNTER_DEFINITIONS_H_

#include <Arduino.h>

namespace SAMD21 
{
#if defined(__SAMD21J15A__) || defined (__SAMD21J15B__) || defined (__SAMD21J16A__) || defined (__SAMD21J16B__) || defined (__SAMD21J17A__) || defined (__SAMD21J18A__)
    typedef TC()
    enum TimerCounterModuleInstances: uint32_t
    {
        Module3 = 0,//reinterpret_cast<uint32_t>(TC3),
        Module4 = 1,//reinterpret_cast<uint32_t>(TC4),
        Module5 = 2,//reinterpret_cast<uint32_t>(TC5),
        Module6 = 3,//reinterpret_cast<uint32_t>(TC6),
        Module7 = 4,//reinterpret_cast<uint32_t>(TC7)
    };
#else
    enum TimerCounterModuleInstances: uint8_t
    {
        Module3 = 0,//reinterpret_cast<uint32_t>(TC3),
        Module4 = 1,//reinterpret_cast<uint32_t>(TC4),
        Module5 = 2//reinterpret_cast<uint32_t>(TC5)
    };
#endif
    enum TimerCounterPrescaler: uint8_t
    {
        DIV1 = 0x0,
        DIV2 = 0x1,
        DIV4 = 0x2,
        DIV8 = 0x3,
        DIV16 = 0x4,
        DIV64 = 0x5,
        DIV256 = 0x6,
        DIV1024 = 0x7
    };

    enum TimerCounterWaveGenerationMode: uint8_t
    {
        NormalFrequency = 0x0,
        MatchFrequency = 0x1,
        NormalPWM = 0x2,
        MatchPWM = 0x3
    };

    enum TimerCounterMode: uint8_t 
    {
        Count8 = 0x1,
        Count16 = 0x0
        //Count32 = 0x2 COUNT 32 NOT YET SUPPORTED, REQUIRES LINKING MODULES
    };
}


#endif