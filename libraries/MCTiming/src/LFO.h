#ifndef _MC_TIMING_LFO_H_
#define _MC_TIMING_LFO_H_

#include <SoftwareLFO.h>
#include "./Tempo.h"

namespace MCTiming
{
    class LFO: public BaseDeluxeLFO
    {
        public:
            void updateTempo(const Tempo& tempo, int32_t mult = 1)
            { 
                this->setIncrementValue(CalculateIncrementValue(tempo, mult));
            };
            static uint32_t CalculateIncrementValue(const Tempo& tempo, int32_t mult);
            static uint32_t GetTempoIncrementRate(const Tempo& tempo);
        protected:
    };
}

#endif