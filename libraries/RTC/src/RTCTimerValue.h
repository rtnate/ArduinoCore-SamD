#ifndef _R_TICK_LIB_H_
#define _R_TICK_LIB_H_

#include "Arduino.h"

class RTCTimerValue
{
    public:
        static constexpr float TICKS_PER_SECOND = (1.f/32768.f);
        static constexpr float TICKS_PER_MICROSECOND = (TICKS_PER_SECOND * 1000000.f);
        static constexpr float TICKS_PER_MILLISECOND = (TICKS_PER_SECOND * 1000.f);
        static constexpr uint32_t MAX_MICROSECONDS = ((float)(UINT32_MAX) / (float)TICKS_PER_MICROSECOND);
        RTCTimerValue(uint32_t val): tickVal(val){};

        uint32_t ticks(){ return tickVal; };

        uint32_t toMicros();
        uint32_t toMillis();
        uint32_t toSeconds(){ return tickVal >> 15; };

        uint32_t toBPM()
        {
            static constexpr float BPM_PER_TICK = (60000.f * (float)TICKS_PER_MILLISECOND);
            return BPM_PER_TICK / tickVal;
        }

        uint32_t toCPUCycles();

        //ASSIGNMENT OPERATOR 
        RTCTimerValue& operator=(const RTCTimerValue& oth)
        {
            tickVal = oth.tickVal;
            return *this;
        }

        RTCTimerValue& operator=(const uint32_t& val)
        {
            tickVal = val;
            return *this;
        }

        RTCTimerValue& operator=(const float& val)
        {
            tickVal = (uint32_t)val;
            return *this;
        }

        
        //MATHMATICAL OPERATIONS

        //ADDITION
        RTCTimerValue& operator+=(const RTCTimerValue& oth)
        {
            tickVal += oth.tickVal;
            return *this;
        }
        RTCTimerValue& operator+=(uint32_t val)
        {
            tickVal += val;
            return *this;
        }
        friend RTCTimerValue operator+(RTCTimerValue lhs, const RTCTimerValue& rhs)
        {
            lhs += rhs;
            return lhs;
        }
        friend RTCTimerValue operator+(RTCTimerValue lhs, const uint32_t& rhs)
        {
            lhs += rhs;
            return lhs;
        }
        friend uint32_t operator+(uint32_t lhs, const RTCTimerValue& rhs)
        {
            return (lhs + rhs.tickVal);
        }

        //SUBTRACTION
        RTCTimerValue& operator-=(const RTCTimerValue& oth)
        {
            tickVal -= oth.tickVal;
            return *this;
        }
        RTCTimerValue& operator-=(uint32_t val)
        {
            tickVal -= val;
            return *this;
        }
        friend RTCTimerValue operator-(RTCTimerValue lhs, const RTCTimerValue& rhs)
        {
            lhs -= rhs;
            return lhs;
        }
        friend RTCTimerValue operator-(RTCTimerValue lhs, const uint32_t& rhs)
        {
            lhs -= rhs;
            return lhs;
        }
        friend uint32_t operator-(uint32_t lhs, const RTCTimerValue& rhs)
        {
            return (lhs - rhs.tickVal);
        }

        //MULTIPLICATION
        RTCTimerValue& operator*=(const RTCTimerValue& oth)
        {
            tickVal *= oth.tickVal;
            return *this;
        }
        RTCTimerValue& operator*=(uint32_t val)
        {
            tickVal *= val;
            return *this;
        }
        friend RTCTimerValue operator*(RTCTimerValue lhs, const RTCTimerValue& rhs)
        {
            lhs *= rhs;
            return lhs;
        }
        friend RTCTimerValue operator*(RTCTimerValue lhs, const uint32_t& rhs)
        {
            lhs *= rhs;
            return lhs;
        }
        friend uint32_t operator*(uint32_t lhs, const RTCTimerValue& rhs)
        {
            return (lhs * rhs.tickVal);
        }

        //DIVISION
        RTCTimerValue& operator/=(const RTCTimerValue& oth)
        {
            tickVal /= oth.tickVal;
            return *this;
        }
        RTCTimerValue& operator/=(uint32_t val)
        {
            tickVal /= val;
            return *this;
        }
        friend RTCTimerValue operator/(RTCTimerValue lhs, const RTCTimerValue& rhs)
        {
            lhs /= rhs;
            return lhs;
        }
        friend RTCTimerValue operator/(RTCTimerValue lhs, const uint32_t& rhs)
        {
            lhs /= rhs;
            return lhs;
        }
        friend uint32_t operator/(uint32_t lhs, const RTCTimerValue& rhs)
        {
            return (lhs / rhs.tickVal);
        }

        //COMPARISONS
        friend inline bool operator< (const RTCTimerValue& lhs, const RTCTimerValue& rhs){ return (lhs.tickVal < rhs.tickVal); };
        friend inline bool operator> (const RTCTimerValue& lhs, const RTCTimerValue& rhs){ return (lhs.tickVal > rhs.tickVal); };
        friend inline bool operator<=(const RTCTimerValue& lhs, const RTCTimerValue& rhs){ return (lhs.tickVal <= rhs.tickVal); };
        friend inline bool operator>=(const RTCTimerValue& lhs, const RTCTimerValue& rhs){ return (lhs.tickVal >= rhs.tickVal); };
        friend inline bool operator==(const RTCTimerValue& lhs, const RTCTimerValue& rhs){ return (lhs.tickVal == rhs.tickVal); };
        friend inline bool operator!=(const RTCTimerValue& lhs, const RTCTimerValue& rhs){ return (lhs.tickVal != rhs.tickVal); };
        //
        friend inline bool operator< (const RTCTimerValue& lhs, const uint32_t& rhs){ return (lhs.tickVal < rhs); };
        friend inline bool operator> (const RTCTimerValue& lhs, const uint32_t& rhs){ return (lhs.tickVal > rhs); };
        friend inline bool operator<=(const RTCTimerValue& lhs, const uint32_t& rhs){ return (lhs.tickVal <= rhs); };
        friend inline bool operator>=(const RTCTimerValue& lhs, const uint32_t& rhs){ return (lhs.tickVal >= rhs); };
        friend inline bool operator==(const RTCTimerValue& lhs, const uint32_t& rhs){ return (lhs.tickVal == rhs); };
        friend inline bool operator!=(const RTCTimerValue& lhs, const uint32_t& rhs){ return (lhs.tickVal != rhs); };
        //
        friend inline bool operator< (const uint32_t& lhs, const RTCTimerValue& rhs){ return (lhs < rhs.tickVal); };
        friend inline bool operator> (const uint32_t& lhs, const RTCTimerValue& rhs){ return (lhs > rhs.tickVal); };
        friend inline bool operator<=(const uint32_t& lhs, const RTCTimerValue& rhs){ return (lhs <= rhs.tickVal); };
        friend inline bool operator>=(const uint32_t& lhs, const RTCTimerValue& rhs){ return (lhs >= rhs.tickVal); };
        friend inline bool operator==(const uint32_t& lhs, const RTCTimerValue& rhs){ return (lhs == rhs.tickVal); };
        friend inline bool operator!=(const uint32_t& lhs, const RTCTimerValue& rhs){ return (lhs != rhs.tickVal); };

        //CASTS
        operator uint32_t() const { return tickVal; };
        operator float() const { return (float)tickVal; };
        operator bool() const { return (bool)tickVal; };
    protected:
        uint32_t tickVal;
};
#endif