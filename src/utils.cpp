#include "utils.hpp"

double vReal[SAMPLES];
double vImag[SAMPLES];
double smoothedHeights[SAMPLES / 2];

volatile int sampleIndex = 0;
volatile bool newDataReady = false;

void init_adc() {
    ADMUX = 0;
    ADMUX |= (1 << REFS0);

    ADCSRA = 0;
    ADCSRB = 0;
    ADCSRA |= (1 << ADEN);
    ADCSRA |= (1 << ADIE);
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1);
}

void init_timer1() {
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);

    // 8000Hz sampling rate
    OCR1A = 1999;
}

void restart_sampling() {
    sampleIndex = 0;
    newDataReady = false;
    TCNT1 = 0;
    TCCR1B |= (1 << CS10);
}

ISR(TIMER1_COMPA_vect) { ADCSRA |= (1 << ADSC); }

ISR(ADC_vect) {
    // Read ADC value and store in vReal
    int low = ADCL;
    int high = ADCH;
    int adcValue = (high << 8) | low;

    if (sampleIndex < SAMPLES) {
        vReal[sampleIndex] = (double)adcValue;
        vImag[sampleIndex] = 0;
        sampleIndex++;
    }

    // Stop the timer
    if (sampleIndex >= SAMPLES) {
        newDataReady = true;
        TCCR1B &= ~(1 << CS10);
    }
}