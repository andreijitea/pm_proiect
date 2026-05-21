#include "fft.hpp"
#include "utils.hpp"
#include "lut.hpp"

#ifdef FFT_FLOAT_NO_LUT
void run_fft(double *vReal, double *vImag) {
    // Bit reversal
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
            // Compute angle directly instead of looking up index
            double theta = -M_PI * i / step;
            double wr = cos(theta);
            double wi = sin(theta);

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
#endif

#ifdef FFT_FLOAT_LUT
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
            uint16_t tableSize = SAMPLES / 4;

            double wr, wi;

            // Calculate wr = cos(theta)
            if (idx == 0)
                wr = 1.0;
            else if (idx < tableSize)
                wr = pgm_read_float(&sinTable[tableSize - idx]);
            else if (idx == tableSize)
                wr = 0.0;
            else
                wr = -pgm_read_float(&sinTable[idx - tableSize]);

            // Calculate wi = -sin(theta)
            if (idx == 0)
                wi = 0.0;
            else if (idx < tableSize)
                wi = -pgm_read_float(&sinTable[idx]);
            else if (idx == tableSize)
                wi = -1.0;
            else
                wi = -pgm_read_float(&sinTable[2 * tableSize - idx]);

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
#endif

#ifdef FFT_INT_LUT
void run_fft(double *vReal, double *vImag) {

    // Scale input up to use int16 range properly
    // Signal after DC removal + windowing is small; *64 gives adequate precision
    int16_t rReal[SAMPLES];
    int16_t rImag[SAMPLES];
    for (uint16_t i = 0; i < SAMPLES; i++) {
        rReal[i] = (int16_t)(vReal[i] * 64.0);
        rImag[i] = 0;
    }

    // Bit reversal
    uint16_t j = 0;
    for (uint16_t i = 0; i < (SAMPLES - 1); i++) {
        if (i < j) {
            int16_t tempR = rReal[i]; rReal[i] = rReal[j]; rReal[j] = tempR;
            int16_t tempI = rImag[i]; rImag[i] = rImag[j]; rImag[j] = tempI;
        }
        uint16_t k = (SAMPLES >> 1);
        while (k <= j) { j -= k; k >>= 1; }
        j += k;
    }

    // Cooley-Tukey FFT with Q15 twiddle factors
    for (uint16_t step = 1; step < SAMPLES; step <<= 1) {
        uint16_t jump = step << 1;
        uint16_t tableSize = SAMPLES / 4;

        // Prevent int16 overflow at each stage from stage 2 onwards
        // Each butterfly can grow values by up to sqrt(2); >>1 keeps headroom
        if (step > 1) {
            for (uint16_t m = 0; m < SAMPLES; m++) {
                rReal[m] >>= 1;
                rImag[m] >>= 1;
            }
        }

        for (uint16_t i = 0; i < step; i++) {
            uint16_t idx = (i * (SAMPLES / 2)) / step;

            int16_t wr, wi;

            // Reconstruct cos/sin from quarter-wave table using symmetry
            // wr = cos(theta), wi = -sin(theta), theta = -pi*i/step
            if (idx == 0) {
                wr = 32767; wi = 0;
            } else if (idx < tableSize) {
                wr =  (int16_t)pgm_read_word(&sinTable[tableSize - idx]);
                wi = -(int16_t)pgm_read_word(&sinTable[idx]);
            } else if (idx == tableSize) {
                wr = 0; wi = -32767;
            } else {
                wr = -(int16_t)pgm_read_word(&sinTable[idx - tableSize]);
                wi = -(int16_t)pgm_read_word(&sinTable[2 * tableSize - idx]);
            }

            for (uint16_t j = i; j < SAMPLES; j += jump) {
                uint16_t k = j + step;

                // Q15 multiply: (int32_t intermediate) >> 15
                int16_t tr = (int16_t)(((int32_t)wr * rReal[k] - (int32_t)wi * rImag[k]) >> 15);
                int16_t ti = (int16_t)(((int32_t)wr * rImag[k] + (int32_t)wi * rReal[k]) >> 15);

                rReal[k] = rReal[j] - tr;
                rImag[k] = rImag[j] - ti;
                rReal[j] += tr;
                rImag[j] += ti;
            }
        }
    }

    // Output scaling:
    // Input was multiplied by 64
    // Right-shifted (log2(SAMPLES) - 1) times: 5 for N=64, 6 for N=128
    // Net scale for N=64:  64 / 32 = 2  → divide by 2
    // Net scale for N=128: 64 / 64 = 1  → divide by 1
    // Use compile-time constant to avoid runtime division
    #if SAMPLES == 64
        const double outputScale = 2.0;
    #elif SAMPLES == 128
        const double outputScale = 1.0;
    #endif

    for (uint16_t i = 0; i < SAMPLES; i++) {
        vReal[i] = (double)rReal[i] / outputScale;
        vImag[i] = (double)rImag[i] / outputScale;
    }
}
#endif