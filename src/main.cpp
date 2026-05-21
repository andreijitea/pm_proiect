#include "config.hpp"
#include "display.hpp"
#include "dsp.hpp"
#include "fft.hpp"
#include "utils.hpp"

#ifdef TESTBENCH_MODE
void run_testbench() {
    // Fill with 1000Hz sine
    float testFreq = 1000.0;
    for (int i = 0; i < SAMPLES; i++) {
        vReal[i] = sin(2.0 * M_PI * testFreq * i / SAMPLING_FREQ) * 200.0;
        vImag[i] = 0;
    }

    apply_window(vReal);

    uint32_t t1 = micros();
    run_fft(vReal, vImag);
    uint32_t t2 = micros();

    calculate_magnitude(vReal, vImag);

    Serial.print("TIME_US:");
    Serial.println(t2 - t1);

    Serial.print("BINS:");
    for (int i = 0; i < SAMPLES / 2; i++) {
        Serial.print(vReal[i]);
        Serial.print(",");
    }
    Serial.println();
}
#endif

int main() {
    // Arduino setup
    init();

    // Project setup
    init_display();
    init_adc();
    init_timer1();
    sei();

// Run testbench if enabled
#ifdef TESTBENCH_MODE
    Serial.begin(115200);
    run_testbench();
    while (true)
        ;
#endif

    // Main loop
    while (true) {
        if (newDataReady) {
            // 1. DC offset removal
            remove_dc_offset(vReal);

            // 2. Windowing
            apply_window(vReal);

            // 3. FFT
            run_fft(vReal, vImag);

            // 4. Transform to Magnitude and Smooth
            calculate_magnitude(vReal, vImag);
            apply_smoothing(vReal, smoothedHeights);

            // 5. Draw
            draw_spectrum();

            // 6. Reset
            restart_sampling();
        }
    }
    return 0;
}
