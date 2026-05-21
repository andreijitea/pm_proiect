#include "dsp.hpp"
#include "fft.hpp"

void remove_dc_offset(double *data) {
    // Calculate the mean
    double mean = 0;
    for (int i = 0; i < SAMPLES; i++) {
        mean += data[i];
    }
    mean /= SAMPLES;

    // Subtract the mean from each sample
    for (int i = 0; i < SAMPLES; i++) {
        data[i] -= mean;
    }
}

void apply_window(double *data) {
    for (int i = 0; i < SAMPLES; i++) {
        data[i] *= pgm_read_float(&hammingWindow[i]);
    }
}

void calculate_magnitude(double *vReal, double *vImag) {
    // Alpha Max Beta Min algorithm
    for (int i = 0; i < (SAMPLES / 2); i++) {
        double real = vReal[i];
        double imag = vImag[i];
        double absReal = real > 0 ? real : -real;
        double absImag = imag > 0 ? imag : -imag;
        double maxVal = absReal > absImag ? absReal : absImag;
        double minVal = absReal > absImag ? absImag : absReal;
        vReal[i] = maxVal + (minVal * 0.25);
    }
}

void apply_smoothing(double *inData, double *outData) {
    for (int i = 0; i < (SAMPLES / 2); i++) {
        double target = inData[i] * GAIN / 10.0;
        outData[i] = (target * SMOOTHING) + (outData[i] * (1.0 - SMOOTHING));
    }
}