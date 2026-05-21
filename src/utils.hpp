#ifndef UTILS_HPP
#define UTILS_HPP

#include "config.hpp"

extern double vReal[SAMPLES];
extern double vImag[SAMPLES];
extern double smoothedHeights[SAMPLES / 2];

extern volatile int sampleIndex;
extern volatile bool newDataReady;

void init_adc();
void init_timer1();
void restart_sampling();

#endif