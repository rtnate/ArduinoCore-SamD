#include "RTCTimerValue.h"


uint32_t RTCTimerValue::toMicros()
{
    static constexpr uint32_t SCALE_FACTOR = 9;
    static constexpr uint32_t MULT_FACTOR = (uint32_t)(TICKS_PER_MICROSECOND * (float)(1 << SCALE_FACTOR));
    if (tickVal > MAX_MICROSECONDS) return MAX_MICROSECONDS;
    //Shift Down 14 bits and Perform the fast method on the upper 18 bits
    uint32_t upper = tickVal >> 14;
    upper *= MULT_FACTOR;
    //Then Scale Back Up To their proper position
    upper <<= (14 - SCALE_FACTOR);
    //Extract the lower 14 bits and perform the fast method on them
    uint32_t lower = tickVal & (0x3FFF);
    lower *= MULT_FACTOR;
    lower >>= SCALE_FACTOR;
    //Return the Two Results Added
    return (upper + lower);
}

uint32_t RTCTimerValue::toMillis()
{
    static constexpr uint32_t SCALE_FACTOR = 12ul;
    static constexpr uint32_t MULT_FACTOR = (uint32_t)(TICKS_PER_MILLISECOND * (float)(1 << SCALE_FACTOR));
    uint32_t upper = tickVal >> 8;
    upper *= MULT_FACTOR;
    upper >>= (SCALE_FACTOR - 8);
    uint32_t lower = tickVal & 0xFF;
    lower *= MULT_FACTOR;
    lower >>= SCALE_FACTOR;
    return (upper + lower);
}
