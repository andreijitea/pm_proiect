#include "fft.hpp"
#include "lut.hpp"
#include "utils.hpp"

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
            // Compute angle
            double theta = -M_PI * i / step;
            double wr = cos(theta);
            double wi = sin(theta);

            // Butterfly computations
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
            // Calculate twiddle factors from quarter-wave sine table
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

            // Butterfly computations
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
    // Reuse vReal and vImag memory to store int16_t values
    // to keep the memory usage below the 2KB RAM limit of ATmega328P
    int16_t *rReal = (int16_t *)vReal;
    int16_t *rImag = (int16_t *)vImag;

    // Scale input up to use int16 range properly
    for (uint16_t i = 0; i < SAMPLES; i++) {
        double realVal = vReal[i];
        rReal[i] = ((int16_t)realVal) << 5; 
        rImag[i] = 0;
    }

    // Bit reversal
    uint16_t j = 0;
    for (uint16_t i = 0; i < (SAMPLES - 1); i++) {
        if (i < j) {
            int16_t tempR = rReal[i];
            rReal[i] = rReal[j];
            rReal[j] = tempR;
            int16_t tempI = rImag[i];
            rImag[i] = rImag[j];
            rImag[j] = tempI;
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

        // Prevent int16 overflow by right-shifting the results
        if (step > 1) {
            for (uint16_t m = 0; m < SAMPLES; m++) {
                rReal[m] >>= 1;
                rImag[m] >>= 1;
            }
        }

        for (uint16_t i = 0; i < step; i++) {
            // Calculate twiddle factors from quarter-wave sine table
            uint16_t idx = (i * (SAMPLES / 2)) / step;
            uint16_t tableSize = SAMPLES / 4;

            int16_t wr, wi;

            // Calculate wr = cos(theta)
            if (idx == 0)
                wr = 32767;
            else if (idx < tableSize)
                wr = (int16_t)pgm_read_word(&sinTable[tableSize - idx]);
            else if (idx == tableSize)
                wr = 0;
            else
                wr = -(int16_t)pgm_read_word(&sinTable[idx - tableSize]);

            // Calculate wi = -sin(theta)
            if (idx == 0)
                wi = 0;
            else if (idx < tableSize)
                wi = -(int16_t)pgm_read_word(&sinTable[idx]);
            else if (idx == tableSize)
                wi = -32767;
            else
                wi = -(int16_t)pgm_read_word(&sinTable[2 * tableSize - idx]);

            // Butterfly computations
            for (uint16_t j = i; j < SAMPLES; j += jump) {
                uint16_t k = j + step;

                // Q15 multiply: (int32_t intermediate) >> 15
                int16_t tr = (int16_t)(((int32_t)wr * rReal[k] -
                                        (int32_t)wi * rImag[k]) >>
                                       15);
                int16_t ti = (int16_t)(((int32_t)wr * rImag[k] +
                                        (int32_t)wi * rReal[k]) >>
                                       15);

                rReal[k] = rReal[j] - tr;
                rImag[k] = rImag[j] - ti;
                rReal[j] += tr;
                rImag[j] += ti;
            }
        }
    }

// Scale output back to double range
// So it has the same magnitude as the float versions of the FFT
#if SAMPLES == 64
    const double outputMultiplier = 1.0;
#elif SAMPLES == 128
    const double outputMultiplier = 2.0;
#endif

    // Reverse loop to avoid overwriting data before it is read
    for (int16_t i = SAMPLES - 1; i >= 0; i--) {
        int16_t outR = rReal[i];
        int16_t outI = rImag[i];
        vReal[i] = (double)outR * outputMultiplier;
        vImag[i] = (double)outI * outputMultiplier;
    }
}
#endif