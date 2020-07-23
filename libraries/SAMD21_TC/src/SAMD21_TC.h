#ifndef _SAMD21_TC_LIB_H_
#define _SAMD21_TC_LIB_H_

#ifdef ARDUINO
#include <Arduino.h>
#endif

#if !(SAMD21_SERIES)
#error "Library SAMD21_TC only supports SAMD Arduino microcontrollers"
#endif

#include "./SAMD21/TimerCounter.h"

#endif