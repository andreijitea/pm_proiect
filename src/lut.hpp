#ifndef LUT_HPP
#define LUT_HPP
#include "config.hpp"

#if defined(FFT_FLOAT_NO_LUT) || defined(FFT_FLOAT_LUT)
#if SAMPLES == 64
const float sinTable[16] PROGMEM = {
    0.000, 0.097, 0.194, 0.290, 0.382, 0.471, 0.555, 0.634,
    0.706, 0.772, 0.831, 0.881, 0.923, 0.956, 0.980, 0.995};
const float hammingWindow[64] PROGMEM = {
    0.080, 0.082, 0.089, 0.100, 0.116, 0.136, 0.160, 0.188, 0.219, 0.253, 0.290,
    0.330, 0.372, 0.415, 0.460, 0.505, 0.551, 0.597, 0.642, 0.686, 0.729, 0.770,
    0.808, 0.844, 0.877, 0.906, 0.932, 0.954, 0.972, 0.986, 0.995, 0.999, 0.999,
    0.995, 0.986, 0.973, 0.955, 0.933, 0.907, 0.878, 0.845, 0.809, 0.771, 0.730,
    0.687, 0.643, 0.598, 0.553, 0.507, 0.461, 0.417, 0.373, 0.331, 0.291, 0.254,
    0.220, 0.188, 0.161, 0.137, 0.117, 0.101, 0.089, 0.082, 0.080};
const float hannWindow[64] PROGMEM = {};
#elif SAMPLES == 128
const float sinTable[32] PROGMEM = {};
const float hammingWindow[128] PROGMEM = {};
const float hannWindow[128] PROGMEM = {};
#endif
#endif

#if defined(FFT_INT_LUT) || defined(FFT_INT_SPLIT_LUT)
#if SAMPLES == 64
const int16_t sinTable[16] PROGMEM = {};
const int16_t hammingWindow[64] PROGMEM = {};
const int16_t hannWindow[64] PROGMEM = {};
#elif SAMPLES == 128
const int16_t sinTable[32] PROGMEM = {};
const int16_t hammingWindow[128] PROGMEM = {};
const int16_t hannWindow[128] PROGMEM = {};
#endif
#endif

#endif