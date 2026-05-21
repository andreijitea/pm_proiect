#ifndef DSP_HPP
#define DSP_HPP
#include "config.hpp"

void remove_dc_offset(double *data);
void apply_window(double *data);
void calculate_magnitude(double *vReal, double *vImag);
void apply_smoothing(double *inData, double *outData);

#endif