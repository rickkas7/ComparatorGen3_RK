// This example counts the number of rising crossing and prints it to debug serial, at most once per second

#include "ComparatorGen3_RK.h"

#include <atomic>

SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler;

// interruptCounter is an atomic variable, so sets and gets occur atomically
std::atomic<uint32_t> interruptCounter;

void comparatorCallback(nrf_lpcomp_event_t event);


void setup() {
    // Wait for a USB serial connection for up to 15 seconds
    // (only necessary for testing so you can see all of the logs)
    waitFor(Serial.isConnected, 15000);
    delay(2000);

    interruptCounter.store(0, std::memory_order_relaxed);

    nrfx_err_t err = ComparatorGen3::instance()
        .withReferenceVoltage(ComparatorGen3::ReferenceVoltage::REF_1V7)
        .withInput(A1)
        .withInterruptMode(FALLING)
        .withHysteresis(false)
        .withEventCallback(comparatorCallback)
        .init();

    Log.info("err=%d", err);


}

void loop() {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 1000) {
        lastUpdate = millis();
        uint32_t count = interruptCounter.fetch_and(0, std::memory_order_relaxed);
        if (count) {
            Log.info("count=%lu", count);
        }
    }
}

void comparatorCallback(nrf_lpcomp_event_t event) {
    // event will be NRF_LPCOMP_EVENT_UP, NRF_LPCOMP_EVENT_DOWN, NRF_LPCOMP_EVENT_CROSS

    // This increments the value atomically. Even if the ISR triggers
    // while we're resetting the value from loop, the count will
    // not be lost.
    interruptCounter.fetch_add(1, std::memory_order_relaxed);
}
