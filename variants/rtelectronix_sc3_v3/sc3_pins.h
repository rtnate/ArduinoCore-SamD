#ifndef _SC_PINS_H_
#define _SC_PINS_H_

#ifndef _VARIANT_ARDUINO_ZERO_
#error "This File (sc3_pins.h) Must Be Included From variant.h"
#endif

#define SC3_LINK_RX (0ul) //!< Pedal Link Serial Rx Pin
#define SC3_LINK_TX (1ul) //!< Pedal Link Serial Tx Pin
#define SC3_CS_SHIFT_REGISTERS (2ul) //!< Shift Registers Chip Select Pin
#define SC3_PWM_R (3ul) //!< Control Voltage PWM Pin Right
#define SC3_PWM_C (4ul) //!< Control Voltage PWM Pin Center
#define SC3_PWM_L (9ul) //!< Control Voltage PWM Pin Left
#define SC3_PRESET_OSC_ENABLE (11ul) //!< Analog Preset Oscillator Enable Pin
#define SC3_MODE_SWITCH_PIN (5ul) //!< Mode Switch Input Pin
#define SC3_TEMPO_MULT_PIN (8ul) //!< Tempo Mult Rotary Switch Input Pin
#define SC3_MIX_PIN A5 //!< Mix Control Input Pin
#define SC3_CHORUS_I_PIN A0 //!< Chorus I Control Input Pin
#define SC3_CHORUS_II_PIN A1 //!< Chourus II Control Input Pin
#define SC3_CHORUS_III_PIN A2 //!< Chorus III Control Input Pin
#define SC3_SPEED_PIN A3 //!< Speed Control Input Pin
#define SC3_WIDTH_PIN A4 //!< Width Control Input Pin
#define SC3_FSW_A_PIN (6ul) //!< Footswitch A (Left) Input Pin
#define SC3_FSW_B_PIN (12ul) //!< Footswitch B (Center) Input Pin
#define SC3_FSW_C_PIN (10ul) //!< Footswitch C (Right) Input Pin
#define SC3_3V_LOWER (7ul) //!< 3.3V To Lower Board Enable Pin
#define SC3_SDA_PIN PIN_WIRE_SDA //!< I2C SDA Line Pin
#define SC3_SCL_PIN PIN_WIRE_SCL //!< I2C SCL Line Pin
#define SC3_MOSI_SHIFT_REGISTERS PIN_SPI_MOSI //!< Shift Registers Data Out Pin
#define SC3_SCK_SHIFT_REGISTERS PIN_SPI_SCK //!< Shift Registers Clock Out Pin
#define SC3_MIDI_TX (30ul) //!< MIDI Serial Tx Pin
#define SC3_MIDI_RX (31ul) //!< MIDI Serial Rx Pin

#define SC3_MIDI_PAD_RX (SERCOM_RX_PAD_3) //!< MIDI Serial Rx Sercom Pad
#define SC3_MIDI_PAD_TX (UART_TX_PAD_2) //!< MIDI Serial Tx Sercom Pad
#define SC3_MIDI_SERCOM sercom5 //!< MIDI Serial Sercom Module Number

#endif 