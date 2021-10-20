#include "ComparatorGen3_RK.h"


//#include "nrf_error.h"


#ifndef LPCOMP_FEATURE_HYST_PRESENT
#error("unexpected, LPCOMP_FEATURE_HYST_PRESENT was not set")
#endif

ComparatorGen3 *ComparatorGen3::_instance;

// [static]
ComparatorGen3 &ComparatorGen3::instance()
{
    if (!_instance)
    {
        _instance = new ComparatorGen3();
    }
    return *_instance;
}

ComparatorGen3::ComparatorGen3()
{
}

ComparatorGen3::~ComparatorGen3()
{
}

nrfx_err_t ComparatorGen3::init() {
    // Attach the interrupt handler to user firmware. One trick to getting the nRF52 SDK to work properly from user application is 
    // detaching the interrupt from the system and attaching it o the user code. The reason is that there are two instances of the 
    // nRF52 SDK, the one initialized by the system, and the one you have just initialized in the user space (when used). 
    // - Both COMP and LPCOMP use the `COMP_LPCOMP_IRQn` IRQ (19). 
    // - The nRF52 drivers use the function `nrfx_lpcomp_irq_handler` for the IRQ handler.
    //
    // These are hidden in the nRF52 SDK source. Every time I need one I just search for the last one I used, because all of the IRQ 
    // constants are defined in one file and handlers in another, and it's easy to just go down the list and find the one for the IRQ 
    // you're looking for.
    // 
    // When you call `attachInterruptDirect` it directs Device OS to map the hardware interrupt (by its _IRQn constant) to a function. 
    // However, since the attachInterruptDirect parameter is referenced from user firmware, the `nrfx_lpcomp_irq_handler` function will 
    // be the one in the user firmware version of the nRF52 SDK, so the interrupt will be mapped appropriately to the user nRF52 driver 
    // in the SDK. This allows the nRF52 driver callbacks to work properly.
    attachInterruptDirect(COMP_LPCOMP_IRQn, nrfx_lpcomp_irq_handler);

    // Also note the nrfx_lpcomp.cpp file. This is unfortunately necessary because by default the nRF52 SDK included with user firmware
    // doesn't have all peripherals enabled in it. LPCOMP is one of the disabled once. But simply including the source file allows it
    // to work, subject to the interrupt situation, above.

    nrfx_err_t err = nrfx_lpcomp_init(&config, eventHandler);
    if (err == 0) {
        if (enableOnInit) {
            enable();
        }
    }

    return err;
}

void ComparatorGen3::enable() {
    nrfx_lpcomp_enable();
}

void ComparatorGen3::disable() {
    nrfx_lpcomp_disable();
}

void ComparatorGen3::uninit() {
    nrfx_lpcomp_uninit();

    detachInterruptDirect(COMP_LPCOMP_IRQn);
}


// Select the input pin for the channel.
// Particle  ADC   nRF    nrf_lpcomp_input_t     notes
// Pin       Chan  Pin
//           0     P0.02  NRF_LPCOMP_INPUT_0  A7 on SoMs
// A0        1     P0.03  NRF_LPCOMP_INPUT_1
// A1        2     P0.04  NRF_LPCOMP_INPUT_2
//           3     P0.05  NRF_LPCOMP_INPUT_3  A6 on SoMs, BAT on Argon and Xenon
// A2        4     P0.28  NRF_LPCOMP_INPUT_4
// A3        5     P0.29  NRF_LPCOMP_INPUT_5
// A4        6     P0.30  NRF_LPCOMP_INPUT_6
// A5        7     P0.31  NRF_LPCOMP_INPUT_7
nrf_lpcomp_input_t ComparatorGen3::particlePinToNRF(pin_t pin) const {
	switch(pin) {
	case A0: return NRF_LPCOMP_INPUT_1;
	case A1: return NRF_LPCOMP_INPUT_2;
	case A2: return NRF_LPCOMP_INPUT_4;
	case A3: return NRF_LPCOMP_INPUT_5;
	case A4: return NRF_LPCOMP_INPUT_6;
	case A5: return NRF_LPCOMP_INPUT_7;
#ifdef A6
	case A6: return NRF_LPCOMP_INPUT_3;
#endif
#ifdef A7
	case A7: return NRF_LPCOMP_INPUT_0;
#endif
	default: return NRF_LPCOMP_INPUT_0;
	}
}

// [static] 
void ComparatorGen3::eventHandler(nrf_lpcomp_event_t event) {
    // This function is called from an ISR context so you must not allocate
    // memory, lock a mutex, or perform lengthy operations in this function!
    if (_instance && _instance->eventCallback) {
        _instance->eventCallback(event);
    }
}



