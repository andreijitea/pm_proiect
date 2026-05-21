#include "utils.hpp"

double vReal[SAMPLES];
double vImag[SAMPLES];
double smoothedHeights[SAMPLES / 2];

volatile int sampleIndex = 0;
volatile bool newDataReady = false;

void init_adc() {
    ADMUX = 0;             // Clear ADMUX; select ADC0 (A0)
    ADMUX |= (1 << REFS0); // Set to AVcc

    ADCSRA = 0;            // Clear ADCSRA
    ADCSRB = 0;            // Clear ADCSRB (No Auto-trigger)
    ADCSRA |= (1 << ADEN); // Enable ADC
    ADCSRA |= (1 << ADIE); // Enable ADC Interrupt
    ADCSRA |= (1 << ADPS2) |
              (1 << ADPS1); // Prescaler 64 for ~125kHz ADC clock (16MHz / 64)
}

void init_timer1() {
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= (1 << WGM12);  // CTC mode
    TCCR1B |= (1 << CS10);   // Prescaler 1
    TIMSK1 |= (1 << OCIE1A); // Enable Timer1 Compare Match A Interrupt!

    OCR1A = 1999; // Compare Match A for 8kHz
}

void restart_sampling() {
    sampleIndex = 0;
    newDataReady = false;
    TCNT1 = 0;             // Reset Timer1 counter
    TCCR1B |= (1 << CS10); // Start Timer1 to securely trigger ADC
}

ISR(TIMER1_COMPA_vect) {
    ADCSRA |= (1 << ADSC); // Start ADC Conversion
}

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

    if (sampleIndex >= SAMPLES) {
        newDataReady = true;
        TCCR1B &= ~(1 << CS10); // Stop Timer1 to halt sampling until next batch
    }
}