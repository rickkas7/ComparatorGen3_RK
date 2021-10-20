// This example toggles the blue D7 LED when a crossing occurs

#include "ComparatorGen3_RK.h"

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler;

bool outputState = false;

pin_t OUTPUT_PIN = D7;

void comparatorCallback(nrf_lpcomp_event_t event);


void setup() {
    pinMode(OUTPUT_PIN, OUTPUT);

    ComparatorGen3::instance()
        .withReferenceVoltage(ComparatorGen3::ReferenceVoltage::REF_0V8)
        .withInput(A1)
        .withInterruptMode(CHANGE)
        .withHysteresis(true)
        .withEventCallback(comparatorCallback)
        .init();
}

void loop() {
}

void comparatorCallback(nrf_lpcomp_event_t event) {
    if (event != NRF_LPCOMP_EVENT_CROSS) {
        return;
    }
    outputState = !outputState;
    digitalWriteFast(OUTPUT_PIN, outputState);
}
