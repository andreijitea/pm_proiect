#include "fft.hpp"
#include "utils.hpp"

void run_fft(double *vReal, double *vImag) {
    // Bit Reversal
    uint16_t j = 0;
    for (uint16_t i = 0; i < (SAMPLES - 1); i++) {
        if (i < j) {
            double tempR = vReal[i];
            vReal[i] = vReal[j];
            vReal[j] = tempR;
            double tempI = vImag[i];
            vImag[i] = vImag[j];
            vImag[j] = tempI;
        }
        uint16_t k = (SAMPLES >> 1);
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }

    // Cooley-Tukey FFT
    for (uint16_t step = 1; step < SAMPLES; step <<= 1) {
        uint16_t jump = step << 1;

        for (uint16_t i = 0; i < step; i++) {
            // The angle step corresponds to pi / step.
            // A full half-circle (pi) maps to SAMPLES / 2 (which is 32
            // positions).
            uint16_t idx = (i * (SAMPLES / 2)) / step;

            double wr, wi;

            // Calculate wr = cos(theta)
            if (idx == 0)
                wr = 1.0;
            else if (idx < 16)
                wr = pgm_read_float(&sinTable[16 - idx]);
            else if (idx == 16)
                wr = 0.0;
            else
                wr = -pgm_read_float(&sinTable[idx - 16]);

            // Calculate wi = -sin(theta)
            if (idx == 0)
                wi = 0.0;
            else if (idx < 16)
                wi = -pgm_read_float(&sinTable[idx]);
            else if (idx == 16)
                wi = -1.0;
            else
                wi = -pgm_read_float(&sinTable[32 - idx]);

            for (uint16_t j = i; j < SAMPLES; j += jump) {
                uint16_t k = j + step;

                double tr = wr * vReal[k] - wi * vImag[k];
                double ti = wr * vImag[k] + wi * vReal[k];

                vReal[k] = vReal[j] - tr;
                vImag[k] = vImag[j] - ti;
                vReal[j] += tr;
                vImag[j] += ti;
            }
        }
    }
}