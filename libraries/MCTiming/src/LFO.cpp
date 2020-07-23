#include "./LFO.h"

using namespace MCTiming;

uint32_t LFO::CalculateIncrementValue(const Tempo& tempo, int32_t mult)
{
    uint32_t countsPerIncrement = GetTempoIncrementRate(tempo);
    if (mult == 0) return countsPerIncrement;
    else if (mult < 0)
    {
        mult = -mult;
        countsPerIncrement = countsPerIncrement / mult;
    }
    else 
    {
        countsPerIncrement = countsPerIncrement * mult;
    }
    return countsPerIncrement;
}

uint32_t LFO::GetTempoIncrementRate(const Tempo& tempo)
{
    float countsPerIncrement = (float)CountsPerPeriod * tempo.getBeatFraction();
    return (uint32_t) countsPerIncrement;
}