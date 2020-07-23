#ifndef _SC_PINS_H_
#define _SC_PINS_H_

#ifndef _VARIANT_ARDUINO_ZERO_
#error "This File (sc3_pins.h) Must Be Included From variant.h"
#endif

#define SC3_LINK_RX (0ul)
#define SC3_LINK_TX (1ul)
#define SC3_CS_SHIFT_REGISTERS (2ul)
#define SC3_PWM_R (3ul)
#define SC3_PWM_C (4ul)
#define SC3_PWM_L (9ul)
#define SC3_PRESET_OSC_ENABLE (11ul)
#define SC3_MODE_SWITCH_PIN (5ul)
#define SC3_TEMPO_MULT_PIN (8ul)
#define SC3_MIX_PIN A5
#define SC3_CHORUS_I_PIN A0
#define SC3_CHORUS_II_PIN A1
#define SC3_CHORUS_III_PIN A2
#define SC3_SPEED_PIN A3
#define SC3_WIDTH_PIN A4
#define SC3_FSW_A_PIN (6ul)
#define SC3_FSW_B_PIN (12ul)
#define SC3_FSW_C_PIN (10ul)
#define SC3_3V_LOWER (7ul)
#define SC3_SDA_PIN PIN_WIRE_SDA
#define SC3_SCL_PIN PIN_WIRE_SCL
#define SC3_MOSI_SHIFT_REGISTERS PIN_SPI_MOSI
#define SC3_SCK_SHIFT_REGISTERS PIN_SPI_SCK
#define SC3_MIDI_TX (30ul)
#define SC3_MIDI_RX (31ul)
#endif 