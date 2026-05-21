#include "display.hpp"
#include "utils.hpp"

U8G2_SSD1306_128X64_NONAME_1_4W_HW_SPI u8g2(U8G2_R0, 10, 9, 8);

void init_display() {
    u8g2.begin();
    for (int i = 0; i < (SAMPLES / 2); i++) {
        smoothedHeights[i] = 0;
    }
}

void draw_spectrum() {
    u8g2.firstPage();
    do {
        // START AT i = 2 to skip 0Hz and 125Hz bins
        for (int i = 2; i < (SAMPLES / 2); i++) {
            int h = (int)smoothedHeights[i];
            if (h > 63)
                h = 63;
            if (h < 2)
                h = 2;

            // Shift the X axis back by 2 bins so they render on the left edge
            int x = (i - 2) * 4;
            u8g2.drawBox(x, 64 - h, 3, h);
        }
    } while (u8g2.nextPage());
}