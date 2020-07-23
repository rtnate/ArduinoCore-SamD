#ifndef _ADC_SCAN_H_
#define _ADC_SCAN_H_

#include <Arduino.h>
#if !(SAMD21_SERIES)
#error "Library ADCScan only supports SAMD Arduino microcontrollers"
#endif

class ADCScan 
{
    public:
        static constexpr uint8_t NO_SCAN_CHANNELS = 16;
        enum Resolution 
        {
            ADCResolution8Bits = 8,
            ADCResolution10Bits = 10,
            ADCResolution12Bits = 12
        };

        enum AveragingMode 
        {
            Average1Sample = 0x0,
            Average2Samples = 0x1,
            Average4Samples = 0x2,
            Average8Samples = 0x3,
            Average16Samples = 0x4,
            Average32Samples = 0x5,
            Average64Samples = 0x6,
            Average128Samples = 0x7,
            Average256Samples = 0x8,
            Average512Samples = 0x9,
            Average1024Samples = 0xA
        };

        ADCScan(): adcData{0}, readIndex(0), enabled(false){};
        bool enablePin(uint8_t pinNo);
        void begin(Resolution resolution = ADCResolution10Bits);
        void initialize(Resolution resolution = ADCResolution10Bits);
        void start();
        void enableAveraging(AveragingMode mode);
        void resultReadyISR();
        uint16_t readChannel(uint8_t ch) const { return adcData[ch]; };
        uint16_t readPin(uint8_t pinNo) const;
    protected:
        uint16_t adcData[16];
        uint8_t readIndex;
        bool enabled;
        void configurePinScan();
        void enableInterrupts();
        void enableADC();
};

#endif