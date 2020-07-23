#ifndef _LIB_RTC_H_
#define _LIB_RTC_H_

#include "Arduino.h"

typedef void (*voidFuncPtr)(void);

enum class RTCMode: uint8_t 
{
    DISABLED = 0,
    STANDARD = 1,
    RTICKS = 2
};

class RTCManager
{
public:
    enum Alarm_Match : uint8_t // Should we have this enum or just use the identifiers from /component/rtc.h ?
    {
        MATCH_OFF = RTC_MODE2_MASK_SEL_OFF_Val,                  // Never
        MATCH_SS = RTC_MODE2_MASK_SEL_SS_Val,                    // Every Minute
        MATCH_MMSS = RTC_MODE2_MASK_SEL_MMSS_Val,                // Every Hour
        MATCH_HHMMSS = RTC_MODE2_MASK_SEL_HHMMSS_Val,            // Every Day
        MATCH_DHHMMSS = RTC_MODE2_MASK_SEL_DDHHMMSS_Val,         // Every Month
        MATCH_MMDDHHMMSS = RTC_MODE2_MASK_SEL_MMDDHHMMSS_Val,    // Every Year
        MATCH_YYMMDDHHMMSS = RTC_MODE2_MASK_SEL_YYMMDDHHMMSS_Val // Once, on a specific date and a specific time
    };



    RTCManager();
    void begin(bool resetTime = false){ begin(RTCMode::STANDARD, resetTime); };
    void begin(RTCMode mode = RTCMode::STANDARD, bool resetTime = false);

    void enableAlarm(Alarm_Match match);
    void disableAlarm();

    void attachInterrupt(voidFuncPtr callback);
    void detachInterrupt();

    void standbyMode();

    /* Get Functions */

    uint8_t getSeconds();
    uint8_t getMinutes();
    uint8_t getHours();

    uint8_t getDay();
    uint8_t getMonth();
    uint8_t getYear();

    uint8_t getAlarmSeconds();
    uint8_t getAlarmMinutes();
    uint8_t getAlarmHours();

    uint8_t getAlarmDay();
    uint8_t getAlarmMonth();
    uint8_t getAlarmYear();

    uint32_t getCounter();

    /* Set Functions */

    void setSeconds(uint8_t seconds);
    void setMinutes(uint8_t minutes);
    void setHours(uint8_t hours);
    void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);

    void setDay(uint8_t day);
    void setMonth(uint8_t month);
    void setYear(uint8_t year);
    void setDate(uint8_t day, uint8_t month, uint8_t year);

    void setAlarmSeconds(uint8_t seconds);
    void setAlarmMinutes(uint8_t minutes);
    void setAlarmHours(uint8_t hours);
    void setAlarmTime(uint8_t hours, uint8_t minutes, uint8_t seconds);

    void setAlarmDay(uint8_t day);
    void setAlarmMonth(uint8_t month);
    void setAlarmYear(uint8_t year);
    void setAlarmDate(uint8_t day, uint8_t month, uint8_t year);

    /* Epoch Functions */

    uint32_t getEpoch();
    uint32_t getY2kEpoch();
    void setEpoch(uint32_t ts);
    void setY2kEpoch(uint32_t ts);
    void setAlarmEpoch(uint32_t ts);

    bool isConfigured()
    {
        return (_mode != RTCMode::DISABLED);
    }

    RTCMode getMode()
    {
        return _mode;
    }

    inline void isr()
    {
        if (_mode == RTCMode::RTICKS)
        {
            _overflow_count += 1;
            RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_OVF; // must clear flag at end
        }
        else RTC->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM0; // must clear flag at end
    }

protected:
    RTCMode _mode;
    uint32_t _overflow_count;

    void initStandard(bool resetTime);
    void initRticks();
    void config32kOSC(void);
    void configureClockStandard(void);
    void configureClockRticks(void);
    void RTCreadRequest();
    bool RTCisSyncing(void);
    void RTCdisable();
    void RTCenable();
    void RTCreset();
    void RTCresetRemove();
    inline void RTAwaitSync()
    {
        while(RTCisSyncing()){};
    }

};

extern RTCManager RealTimeClock;

#endif