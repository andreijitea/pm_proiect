#include "config.hpp"
#include "display.hpp"
#include "dsp.hpp"
#include "fft.hpp"
#include "utils.hpp"

int main() {
    init(); // Arduino built-in initialization

    // SETUP
    init_display();
    init_adc();
    init_timer1();
    sei(); // Enable interrupts

    // MAIN LOOP
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
