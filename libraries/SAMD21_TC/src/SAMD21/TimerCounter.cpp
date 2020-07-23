#include "TimerCounter.h"

using namespace SAMD21;

TimerCounter TimerCounter::module(TimerCounter::ModuleSelection moduleNo)
{
    static constexpr Tc* modules[] = TC_INSTS;
    auto tc = modules[moduleNo];
    return TimerCounter(*tc);
}

void TimerCounter::reset()
{
    //Set The Reset Bit
    tc.COUNT16.CTRLA.bit.SWRST = 1;
    //Wait For Sync
    while(tc.COUNT16.STATUS.bit.SYNCBUSY){};
}

void TimerCounter::begin(TimerCounter::CountMode mode /* = Count16 */, 
                        TimerCounter::Prescaler prescaler /* = DIV1 */, 
                        TimerCounter::WaveGenerationMode waveMode /* = NormalFrequency */, 
                        bool runInStandBy /* = false */)
{
    init(mode, prescaler, waveMode, runInStandBy);
    enable();
}

void TimerCounter::init(TimerCounter::CountMode mode /* = Count16 */, 
                        TimerCounter::Prescaler prescaler /* = DIV1 */, 
                        TimerCounter::WaveGenerationMode waveMode /* = NormalFrequency */, 
                        bool runInStandBy /* = false */)
{
    Serial.print("Enabling Timer Module: ");
    Serial.println((uint32_t)(&tc), HEX);
    if (isEnabled()) 
        reset();
    else 
        initClocks();

    if (mode == Count8)
    {
        //Reset Counter On Prescaler Clock
        tc.COUNT8.CTRLA.bit.PRESCSYNC = TC_CTRLA_PRESCSYNC_PRESC_Val;
        tc.COUNT8.CTRLA.bit.PRESCALER = prescaler;
        tc.COUNT8.CTRLA.bit.WAVEGEN = waveMode;
        tc.COUNT8.CTRLA.bit.MODE = mode;
        if (runInStandBy) tc.COUNT8.CTRLA.bit.RUNSTDBY = 1;
    }
    else 
    {
        //Reset Counter On Prescaler Clock
        tc.COUNT16.CTRLA.bit.PRESCSYNC = TC_CTRLA_PRESCSYNC_PRESC_Val;
        tc.COUNT16.CTRLA.bit.PRESCALER = prescaler;
        tc.COUNT16.CTRLA.bit.WAVEGEN = waveMode;
        tc.COUNT16.CTRLA.bit.MODE = mode;
        if (runInStandBy) tc.COUNT16.CTRLA.bit.RUNSTDBY = 1;
    }
    Serial.println("Config Written");
}

void TimerCounter::enable()
{
    Serial.print("Enabling timer......");
    //CTRLA AND STATUS REGISTERS ARE THE SAME FOR COUNT16 OR COUNT8
    tc.COUNT16.CTRLA.bit.ENABLE = 1;
    while(tc.COUNT16.STATUS.bit.SYNCBUSY){};
    Serial.println("Done");
}

void TimerCounter::setCompareChannel0(uint16_t data)
{
    if (getMode() == Count8)
    {
        uint8_t truncData = (uint8_t)data;
        tc.COUNT8.CC[0].reg = truncData;
        while(tc.COUNT8.STATUS.bit.SYNCBUSY){};
    }
    else 
    {
        tc.COUNT16.CC[0].reg = data;
        while(tc.COUNT16.STATUS.bit.SYNCBUSY){};
    }
}
void TimerCounter::setCompareChannel1(uint16_t data)
{
    if (getMode() == Count8)
    {
        uint8_t truncData = (uint8_t)data;
        tc.COUNT8.CC[1].reg = truncData;
        while(tc.COUNT8.STATUS.bit.SYNCBUSY){};
    }
    else 
    {
        tc.COUNT16.CC[1].reg = data;
        while(tc.COUNT16.STATUS.bit.SYNCBUSY){};
    }
}

void TimerCounter::setPeriod(uint8_t data)
{
    if (getMode() == Count8)
    {
        uint8_t truncData = (uint8_t)data;
        tc.COUNT8.PER.reg = truncData;
        while(tc.COUNT8.STATUS.bit.SYNCBUSY){};
    }
}

void TimerCounter::setCounter(uint16_t data)
{
    if (getMode() == Count8)
    {
        uint8_t truncData = (uint8_t)data;
        tc.COUNT8.COUNT.reg = truncData;
        while(tc.COUNT8.STATUS.bit.SYNCBUSY){};
    }
    else 
    {
        tc.COUNT16.COUNT.reg = data;
        while(tc.COUNT16.STATUS.bit.SYNCBUSY){};
    }
}

void TimerCounter::initClocks()
{
    auto tcAddress = &tc;
    uint32_t gclkID = 0x0;
    if (tcAddress == TC3) gclkID = GCM_TCC2_TC3;
    else gclkID = GCM_TC4_TC5;
    Serial.print("Initializing Timer GCLK...");
    GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | gclkID);
	while (GCLK->STATUS.bit.SYNCBUSY == 1);
    Serial.println("done");
}

void TimerCounter::enableNVIC()
{
    auto tcAddress = &tc;
    IRQn irqNo;
    if (tcAddress == TC3) irqNo = TC3_IRQn;
    else if (tcAddress == TC4) irqNo = TC4_IRQn;
    else if (tcAddress == TC5) irqNo = TC5_IRQn;
#if (TC_INST_NUM >= 4)
    else if (tcAddress == TC6) irqNo = TC6_IRQn;
#endif
#if (TC_INST_NUM >= 5)
    else if (tcAddress == TC7) irqNo = TC7_IRQn;
#endif
    else return;
    NVIC_ClearPendingIRQ(irqNo);
    NVIC_SetPriority(irqNo, InterruptPrioity);
    NVIC_EnableIRQ(irqNo);
}


void TimerCounter::enableInterrupts(bool compareMatch0, bool compareMatch1, bool overflow, 
                                    bool error /* = false */, bool syncReady /* = false */)
{
    uint32_t int_set = 0;
    if (compareMatch0) int_set |= TC_INTENSET_MC0;
    if (compareMatch1) int_set |= TC_INTENSET_MC1;
    if (overflow) int_set |= TC_INTENSET_OVF;
    if (error) int_set |= TC_INTENSET_ERR;
    if (syncReady) int_set |= TC_INTENSET_SYNCRDY;
    if (int_set)
    {
        enableNVIC();
        tc.COUNT16.INTENSET.reg = int_set;
    }
    //COUNT8 and COUNT16 use the same registers for interrupts
}

uint16_t TimerCounter::getCount()
{
    uint16_t val;
    if (mode == Count16)
    {  
        tc.COUNT16.READREQ.reg = TC_READREQ_RREQ | (TC_READREQ_ADDR(TC_COUNT16_COUNT_OFFSET));
        while(tc.COUNT16.STATUS.bit.SYNCBUSY){};
        auto tcval = tc.COUNT16.COUNT.reg;
        val = tcval;
    } 
    else
    {
        tc.COUNT8.READREQ.reg = TC_READREQ_RREQ | (TC_READREQ_ADDR(TC_COUNT8_COUNT_OFFSET));
        while(tc.COUNT8.STATUS.bit.SYNCBUSY){};
        auto tcval = tc.COUNT8.COUNT.reg;
        val = tcval;
    }
    return val;
}