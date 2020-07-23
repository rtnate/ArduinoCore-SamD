#include <time.h>
#include "RTCManager.h"

#define EPOCH_TIME_OFF      946684800  // This is 1st January 2000, 00:00:00 in epoch time
#define EPOCH_TIME_YEAR_OFF 100        // years since 1900

// Default date & time after reset
#define DEFAULT_YEAR    2000    // 2000..2063
#define DEFAULT_MONTH   1       // 1..12
#define DEFAULT_DAY     1       // 1..31
#define DEFAULT_HOUR    0       // 1..23
#define DEFAULT_MINUTE  0       // 0..59
#define DEFAULT_SECOND  0       // 0..59

RTCManager RealTimeClock;
voidFuncPtr RTC_callBack = NULL;

RTCManager::RTCManager(): _mode(RTCMode::DISABLED)
{
}

void RTCManager::begin(RTCMode mode, bool resetTime)
{
    if (mode == RTCMode::RTICKS) initRticks();
    else if (mode == RTCMode::STANDARD) initStandard(resetTime);
    _mode = mode;
    _overflow_count = 0;
}

static inline bool resetReasonNotPORorBOD()
{
    return (PM->RCAUSE.reg & (PM_RCAUSE_SYST | PM_RCAUSE_WDT | PM_RCAUSE_EXT));
}

static inline bool resetClock(bool shouldReset, RTC_MODE2_CLOCK_Type &oldTime)
{
    // If system reset reason was
    // not due to POR or BOD, preserve the clock time
    // POR causes a reset anyway, BOD behaviour is?
    if ((!shouldReset) && resetReasonNotPORorBOD()) 
    {
        if (RTC->MODE2.CTRL.reg & RTC_MODE2_CTRL_MODE_CLOCK) 
        {
            oldTime.reg = RTC->MODE2.CLOCK.reg;
            return true;
        }
    }
    return false;
}

static inline bool shouldPreserveTime(bool shouldReset, bool timeValid, RTC_MODE2_CLOCK_Type &previousTime)
{
    return ((!shouldReset) && (timeValid) && (previousTime.reg != 0L));
}

static inline uint32_t getDefaultTime()
{
    return (RTC_MODE2_CLOCK_YEAR(DEFAULT_YEAR - 2000) | RTC_MODE2_CLOCK_MONTH(DEFAULT_MONTH) 
        | RTC_MODE2_CLOCK_DAY(DEFAULT_DAY) | RTC_MODE2_CLOCK_HOUR(DEFAULT_HOUR) 
        | RTC_MODE2_CLOCK_MINUTE(DEFAULT_MINUTE) | RTC_MODE2_CLOCK_SECOND(DEFAULT_SECOND));

} 

void RTCManager::initStandard(bool resetTime)
{
    uint16_t tmp_reg = 0;
    PM->APBAMASK.reg |= PM_APBAMASK_RTC; // turn on digital interface clock
    config32kOSC(); // enable the 32K crystal oscillator

    RTC_MODE2_CLOCK_Type oldTime;
    bool validTime = resetClock(resetTime, oldTime);

    // Setup clock GCLK2 with OSC32K divided by 32
    configureClockStandard();
    RTCdisable();
    RTCreset();

    //Configure The Real Time Clock For Standard Time format
    tmp_reg |= RTC_MODE2_CTRL_MODE_CLOCK; // set clock operating mode
    tmp_reg |= RTC_MODE2_CTRL_PRESCALER_DIV1024; // set prescaler to 1024 for MODE2
    tmp_reg &= ~RTC_MODE2_CTRL_MATCHCLR; // disable clear on match 
    //According to the datasheet RTC_MODE2_CTRL_CLKREP = 0 for 24h
    tmp_reg &= ~RTC_MODE2_CTRL_CLKREP; // 24h time representation

    RTC->MODE2.READREQ.reg &= ~RTC_READREQ_RCONT; // disable continuously mode
    RTC->MODE2.CTRL.reg = tmp_reg;
    RTAwaitSync();
    //Enable RTC Interrupt
    NVIC_EnableIRQ(RTC_IRQn);
    NVIC_SetPriority(RTC_IRQn, 0x00);
    RTC->MODE2.INTENSET.reg |= RTC_MODE2_INTENSET_ALARM0; // enable alarm interrupt
    RTC->MODE2.Mode2Alarm[0].MASK.bit.SEL = MATCH_OFF; // default alarm match is off (disabled)
    RTAwaitSync();

    RTCenable();
    RTCresetRemove();

    // If desired and valid, restore the time value, else use first valid time value
    if (shouldPreserveTime(resetTime, validTime, oldTime)) 
    {
        RTC->MODE2.CLOCK.reg = oldTime.reg;
    }
    else 
    {
        RTC->MODE2.CLOCK.reg = getDefaultTime();
    }
    RTAwaitSync();
}

void RTCManager::initRticks()
{
    uint16_t tmp_reg = 0;
    PM->APBAMASK.reg |= PM_APBAMASK_RTC; // turn on digital interface clock
    config32kOSC(); // enable the 32K crystal oscillator

    // Setup clock GCLK2 with OSC32K divided by 32
    configureClockStandard();
    RTCdisable();
    RTCreset();

    //Configure The Real Time Clock For Ticks  format (MODE 0)
    tmp_reg |= RTC_MODE0_CTRL_MODE_COUNT32; // set as 32 counter
    tmp_reg |= RTC_MODE0_CTRL_PRESCALER_DIV1; // set prescaler to 1 
    tmp_reg &= ~RTC_MODE0_CTRL_MATCHCLR; // disable clear on match 

    RTC->MODE0.READREQ.reg &= ~RTC_READREQ_RCONT; // disable continuously mode
    RTC->MODE0.CTRL.reg = tmp_reg;
    RTAwaitSync();
    //Enable RTC Interrupt
    NVIC_EnableIRQ(RTC_IRQn);
    NVIC_SetPriority(RTC_IRQn, 0x00);
    //Enable Interrupt On Overflow Only
    RTC->MODE0.EVCTRL.reg = RTC_MODE0_EVCTRL_OVFEO; // enable alarm interrupt
    RTAwaitSync();

    RTCenable();
    RTCresetRemove();
    RTAwaitSync();
}

void RTC_Handler(void)
{
  if (RTC_callBack != NULL) {
    RTC_callBack();
  }
  RealTimeClock.isr();
}

void RTCManager::enableAlarm(Alarm_Match match)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.Mode2Alarm[0].MASK.bit.SEL = match;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::disableAlarm()
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.Mode2Alarm[0].MASK.bit.SEL = 0x00;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::attachInterrupt(voidFuncPtr callback)
{
  RTC_callBack = callback;
}

void RTCManager::detachInterrupt()
{
  RTC_callBack = NULL;
}

void RTCManager::standbyMode()
{
  // Entering standby mode when connected
  // via the native USB port causes issues.
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __DSB();
  __WFI();
}

/*
 * Get Functions
 */

uint8_t RTCManager::getSeconds()
{
  RTCreadRequest();
  return RTC->MODE2.CLOCK.bit.SECOND;
}

uint8_t RTCManager::getMinutes()
{
  RTCreadRequest();
  return RTC->MODE2.CLOCK.bit.MINUTE;
}

uint8_t RTCManager::getHours()
{
  RTCreadRequest();
  return RTC->MODE2.CLOCK.bit.HOUR;
}

uint8_t RTCManager::getDay()
{
  RTCreadRequest();
  return RTC->MODE2.CLOCK.bit.DAY;
}

uint8_t RTCManager::getMonth()
{
  RTCreadRequest();
  return RTC->MODE2.CLOCK.bit.MONTH;
}

uint8_t RTCManager::getYear()
{
  RTCreadRequest();
  return RTC->MODE2.CLOCK.bit.YEAR;
}

uint8_t RTCManager::getAlarmSeconds()
{
  return RTC->MODE2.Mode2Alarm[0].ALARM.bit.SECOND;
}

uint8_t RTCManager::getAlarmMinutes()
{
  return RTC->MODE2.Mode2Alarm[0].ALARM.bit.MINUTE;
}

uint8_t RTCManager::getAlarmHours()
{
  return RTC->MODE2.Mode2Alarm[0].ALARM.bit.HOUR;
}

uint8_t RTCManager::getAlarmDay()
{
  return RTC->MODE2.Mode2Alarm[0].ALARM.bit.DAY;
}

uint8_t RTCManager::getAlarmMonth()
{
  return RTC->MODE2.Mode2Alarm[0].ALARM.bit.MONTH;
}

uint8_t RTCManager::getAlarmYear()
{
  return RTC->MODE2.Mode2Alarm[0].ALARM.bit.YEAR;
}

uint32_t RTCManager::getCounter()
{
  RTCreadRequest();
  return RTC->MODE0.COUNT.reg;
}

/*
 * Set Functions
 */

void RTCManager::setSeconds(uint8_t seconds)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.CLOCK.bit.SECOND = seconds;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setMinutes(uint8_t minutes)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.CLOCK.bit.MINUTE = minutes;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setHours(uint8_t hours)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.CLOCK.bit.HOUR = hours;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setTime(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  if (_mode == RTCMode::STANDARD) {
    setSeconds(seconds);
    setMinutes(minutes);
    setHours(hours);
  }
}

void RTCManager::setDay(uint8_t day)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.CLOCK.bit.DAY = day;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setMonth(uint8_t month)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.CLOCK.bit.MONTH = month;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setYear(uint8_t year)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.CLOCK.bit.YEAR = year;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setDate(uint8_t day, uint8_t month, uint8_t year)
{
  if (_mode == RTCMode::STANDARD) {
    setDay(day);
    setMonth(month);
    setYear(year);
  }
}

void RTCManager::setAlarmSeconds(uint8_t seconds)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.Mode2Alarm[0].ALARM.bit.SECOND = seconds;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setAlarmMinutes(uint8_t minutes)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.Mode2Alarm[0].ALARM.bit.MINUTE = minutes;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setAlarmHours(uint8_t hours)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.Mode2Alarm[0].ALARM.bit.HOUR = hours;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setAlarmTime(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  if (_mode == RTCMode::STANDARD) {
    setAlarmSeconds(seconds);
    setAlarmMinutes(minutes);
    setAlarmHours(hours);
  }
}

void RTCManager::setAlarmDay(uint8_t day)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.Mode2Alarm[0].ALARM.bit.DAY = day;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setAlarmMonth(uint8_t month)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.Mode2Alarm[0].ALARM.bit.MONTH = month;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setAlarmYear(uint8_t year)
{
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.Mode2Alarm[0].ALARM.bit.YEAR = year;
    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setAlarmDate(uint8_t day, uint8_t month, uint8_t year)
{
  if (_mode == RTCMode::STANDARD) {
    setAlarmDay(day);
    setAlarmMonth(month);
    setAlarmYear(year);
  }
}

uint32_t RTCManager::getEpoch()
{
  RTCreadRequest();
  RTC_MODE2_CLOCK_Type clockTime;
  clockTime.reg = RTC->MODE2.CLOCK.reg;

  struct tm tm;

  tm.tm_isdst = -1;
  tm.tm_yday = 0;
  tm.tm_wday = 0;
  tm.tm_year = clockTime.bit.YEAR + EPOCH_TIME_YEAR_OFF;
  tm.tm_mon = clockTime.bit.MONTH - 1;
  tm.tm_mday = clockTime.bit.DAY;
  tm.tm_hour = clockTime.bit.HOUR;
  tm.tm_min = clockTime.bit.MINUTE;
  tm.tm_sec = clockTime.bit.SECOND;

  return mktime(&tm);
}

uint32_t RTCManager::getY2kEpoch()
{
  return (getEpoch() - EPOCH_TIME_OFF);
}

void RTCManager::setAlarmEpoch(uint32_t ts)
{
  if (_mode == RTCMode::STANDARD) {
    if (ts < EPOCH_TIME_OFF) {
      ts = EPOCH_TIME_OFF;
    }

    time_t t = ts;
    struct tm* tmp = gmtime(&t);

    setAlarmDate(tmp->tm_mday, tmp->tm_mon + 1, tmp->tm_year - EPOCH_TIME_YEAR_OFF);
    setAlarmTime(tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
  }
}

void RTCManager::setEpoch(uint32_t ts)
{
  if (_mode == RTCMode::STANDARD) {
    if (ts < EPOCH_TIME_OFF) {
      ts = EPOCH_TIME_OFF;
    }

    time_t t = ts;
    struct tm* tmp = gmtime(&t);

    RTC_MODE2_CLOCK_Type clockTime;

    clockTime.bit.YEAR = tmp->tm_year - EPOCH_TIME_YEAR_OFF;
    clockTime.bit.MONTH = tmp->tm_mon + 1;
    clockTime.bit.DAY = tmp->tm_mday;
    clockTime.bit.HOUR = tmp->tm_hour;
    clockTime.bit.MINUTE = tmp->tm_min;
    clockTime.bit.SECOND = tmp->tm_sec;

    RTC->MODE2.CLOCK.reg = clockTime.reg;

    while (RTCisSyncing())
      ;
  }
}

void RTCManager::setY2kEpoch(uint32_t ts)
{
  if (_mode == RTCMode::STANDARD) {
    setEpoch(ts + EPOCH_TIME_OFF);
  }
}

/* Attach peripheral clock to 32k oscillator */
void RTCManager::configureClockStandard() {
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(2)|GCLK_GENDIV_DIV(4);
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
    ;
#ifdef CRYSTALLESS
  GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL );
#else
  GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL );
#endif
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
    ;
  GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | (RTC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;
}

void RTCManager::configureClockRticks()
{
    //Use Generic Clock 1 (XOSC32K at synchrous frequency)
    GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1 | (RTC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
    while (GCLK->STATUS.bit.SYNCBUSY);
}

/*
 * Private Utility Functions
 */

/* Configure the 32768Hz Oscillator */
void RTCManager::config32kOSC() 
{
#ifndef CRYSTALLESS
  SYSCTRL->XOSC32K.reg = SYSCTRL_XOSC32K_ONDEMAND |
                         SYSCTRL_XOSC32K_RUNSTDBY |
                         SYSCTRL_XOSC32K_EN32K |
                         SYSCTRL_XOSC32K_XTALEN |
                         SYSCTRL_XOSC32K_STARTUP(6) |
                         SYSCTRL_XOSC32K_ENABLE;
#endif
}

/* Synchronise the CLOCK register for reading*/
inline void RTCManager::RTCreadRequest() {
  if (_mode == RTCMode::STANDARD) {
    RTC->MODE2.READREQ.reg = RTC_READREQ_RREQ;
    while (RTCisSyncing())
      ;
  }
}

/* Wait for sync in write operations */
inline bool RTCManager::RTCisSyncing()
{
  return (RTC->MODE2.STATUS.bit.SYNCBUSY);
}

void RTCManager::RTCdisable()
{
  RTC->MODE2.CTRL.reg &= ~RTC_MODE2_CTRL_ENABLE; // disable RTC
  while (RTCisSyncing())
    ;
}

void RTCManager::RTCenable()
{
  RTC->MODE2.CTRL.reg |= RTC_MODE2_CTRL_ENABLE; // enable RTC
  while (RTCisSyncing())
    ;
}

void RTCManager::RTCreset()
{
  RTC->MODE2.CTRL.reg |= RTC_MODE2_CTRL_SWRST; // software reset
  while (RTCisSyncing())
    ;
}

void RTCManager::RTCresetRemove()
{
  RTC->MODE2.CTRL.reg &= ~RTC_MODE2_CTRL_SWRST; // software reset remove
  while (RTCisSyncing())
    ;
}
