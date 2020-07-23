#include "ADCScan.h"
#include "wiring_private.h"

static __inline__ void syncADC() __attribute__((always_inline, unused));
static void syncADC() {
  while (ADC->STATUS.bit.SYNCBUSY == 1)
    ;
}

bool ADCScan::enablePin(uint8_t pinNo)
{
    if (g_APinDescription[pinNo].ulADCChannelNumber == No_ADC_Channel) return false;
    pinPeripheral(pinNo, PIO_ANALOG);
    return true;
}

void ADCScan::begin(Resolution resolution)
{
    initialize(resolution);
    start();
}

void ADCScan::initialize(Resolution resolution)
{
    int res = static_cast<int>(resolution);
    analogReadResolution(res);
    configurePinScan();
}

void ADCScan::enableAveraging(AveragingMode mode)
{
    ADC_AVGCTRL_Type avctl;
    avctl.reg = ADC->AVGCTRL.reg;
    switch (mode)
    {
        case Average1Sample:
            avctl.bit.ADJRES = 0;
            avctl.bit.SAMPLENUM = static_cast<uint8_t>(mode);
            break;
        case Average2Samples:
            avctl.bit.ADJRES = 0x1;
            avctl.bit.SAMPLENUM = static_cast<uint8_t>(mode);
            break;
        case Average4Samples:
            avctl.bit.ADJRES = 0x2;
            avctl.bit.SAMPLENUM = static_cast<uint8_t>(mode);
            break;
        case Average8Samples:
            avctl.bit.ADJRES = 0x3;
            avctl.bit.SAMPLENUM = static_cast<uint8_t>(mode);
            break;
        case Average16Samples:
        case Average32Samples:
        case Average64Samples:
        case Average128Samples:
        case Average256Samples:
        case Average512Samples:
        case Average1024Samples:
            avctl.bit.ADJRES = 0x4;
            avctl.bit.SAMPLENUM = static_cast<uint8_t>(mode);
            break;
        default:
            break;
    }
    ADC->AVGCTRL.reg = avctl.reg;
    syncADC();
    //For Averaging, Resolution Must Be 12 Bits 
    ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_12BIT;
    syncADC();
}

void ADCScan::start()
{
    //Enable ADC
    enableADC();
}

uint16_t ADCScan::readPin(uint8_t pinNo) const 
{
    if (pinNo > PINS_COUNT) return 0;
    auto adcChannel = g_APinDescription[pinNo].ulADCChannelNumber;
    if (adcChannel == No_ADC_Channel) return 0;
    uint8_t ch = static_cast<uint8_t>(adcChannel);
    return readChannel(ch);
}


void ADCScan::configurePinScan()
{
    Serial.print("Enable ADC Pin Scan....");
    //Disable ADC before reconfiguring
    syncADC();
    ADC->CTRLA.bit.ENABLE = 0x00;
    syncADC();
    ADC_INPUTCTRL_Type inputControl;
    inputControl.reg = ADC->INPUTCTRL.reg;
    //Start Scan at Channel 0
    inputControl.bit.INPUTOFFSET = 0x0;
    inputControl.bit.MUXPOS = 0x0;
    //Set Negative Mux to Ground
    inputControl.bit.MUXNEG = ADC_INPUTCTRL_MUXNEG_GND_Val;
    //Scan 16 Channels (0xF)
    constexpr uint8_t SCAN_CH_VAL = NO_SCAN_CHANNELS - 1;
    inputControl.bit.INPUTSCAN = SCAN_CH_VAL;
    //Write the register
    ADC->INPUTCTRL.reg = inputControl.reg;
    syncADC();
    //Set to free running mode 
    //ADC->CTRLB.bit.FREERUN = 0x1;
    //syncADC();
    //Enable Interrupts
    enableInterrupts();
    Serial.println("done.");
}

void ADCScan::enableInterrupts()
{
    //Enable Interrupt On Conversion Ready
    ADC->INTENSET.bit.RESRDY = 0x1;
    //Enable NVIC
    NVIC_ClearPendingIRQ(ADC_IRQn);
    //ADC Priority is Low
    NVIC_SetPriority(ADC_IRQn, 0x0);
    //Enable NVIC Interrupts
    NVIC_EnableIRQ(ADC_IRQn);
}

void ADCScan::enableADC()
{
    //Enable The Module
    ADC->CTRLA.bit.ENABLE = 0x1;
    syncADC();
    //Start the Conversion
    ADC->SWTRIG.bit.START = 1;
}

void ADCScan::resultReadyISR()
{
    if (ADC->INTFLAG.bit.RESRDY == 0x1)
    {
        auto result = ADC->RESULT.reg;
        adcData[readIndex] = result;
        readIndex++;
        readIndex %= NO_SCAN_CHANNELS;
        ADC->INTFLAG.bit.RESRDY = 0x1;
        ADC->SWTRIG.bit.START = 1;
    }
}