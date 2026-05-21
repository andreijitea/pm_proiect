#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <Arduino.h>
#include <U8g2lib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#define SAMPLES 64
#define SAMPLING_FREQ 8000
#define GAIN 12
#define SMOOTHING 0.2

// Pick ONE, comment out the rest
#define FFT_FLOAT_LUT
// #define FFT_FLOAT_NOLUT
// #define FFT_INT_LUT
// #define FFT_INT_SPLITRADIX

#endif