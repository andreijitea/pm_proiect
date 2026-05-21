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

// FFT Implementation Options
#define FFT_FLOAT_NO_LUT
// #define FFT_FLOAT_LUT

// Windowing Options
// #define USE_RECTANGULAR_WINDOW
#define USE_HAMMING_WINDOW
// #define USE_HANN_WINDOW

#endif