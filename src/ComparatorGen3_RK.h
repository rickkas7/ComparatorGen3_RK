#ifndef __COMPARATORGEN3_RK_H
#define __COMPARATORGEN3_RK_H

// Repository: https://github.com/rickkas7/ComparatorGen3_RK
// License: MIT

#include "Particle.h"
 
#include "nrfx_lpcomp.h"

/**
 * This class is a singleton; you do not create one as a global, on the stack, or with new.
 */
class ComparatorGen3
{
public:
    enum class ReferenceVoltage {
        REF_0V4 = NRF_LPCOMP_REF_SUPPLY_1_8,
        REF_0V8 = NRF_LPCOMP_REF_SUPPLY_2_8,
        REF_1V2 = NRF_LPCOMP_REF_SUPPLY_3_8,
        REF_1V7 = NRF_LPCOMP_REF_SUPPLY_4_8,
        REF_2V1 = NRF_LPCOMP_REF_SUPPLY_5_8,
        REF_2V5 = NRF_LPCOMP_REF_SUPPLY_6_8,
        REF_2V9 = NRF_LPCOMP_REF_SUPPLY_7_8,
        PIN_A0  = NRF_LPCOMP_CONFIG_REF_EXT_REF1
    };

    /**
     * @brief Gets the singleton instance of this class, allocating it if necessary
     * 
     * Use ComparatorGen3::instance() to instantiate the singleton.
     */
    static ComparatorGen3 &instance();

    ComparatorGen3 &withReferenceVoltage(ReferenceVoltage ref) {
        config.hal.reference = (nrf_lpcomp_ref_t) ref;
        return *this;
    }

    /**
     * @brief Selects the analog input
     * 
     * @param pin The Particle pin (A0 - A5) to use.
     */
    ComparatorGen3 &withInput(pin_t pin) {
        pinMode(pin, AN_INPUT);
        config.input = particlePinToNRF(pin);
        return *this;
    }

    /**
     * @brief Sets the interrupt mode
     * 
     * @param mode The Particle-style interrupt mode: RISING, FALLING, or CHANGE
     */
    ComparatorGen3 &withInterruptMode(InterruptMode mode) {
        switch(mode) {
            case RISING:
                config.hal.detection = NRF_LPCOMP_DETECT_UP;
                break;
            case FALLING:
                config.hal.detection = NRF_LPCOMP_DETECT_DOWN;
                break;

            // case CHANGE:
            default:
                config.hal.detection = NRF_LPCOMP_DETECT_CROSS;
                break;
        }
        return *this;
    }

    /**
     * @brief Enable 50 mV hysteresis mode. Default is disabled.
     */
    ComparatorGen3 &withHysteresis(bool enable) {
        config.hal.hyst = (enable ? NRF_LPCOMP_HYST_50mV : NRF_LPCOMP_HYST_NOHYST);
        return *this;
    }

    /**
     * @brief Set the interrupt priority. Default is NRFX_LPCOMP_CONFIG_IRQ_PRIORITY (6).
     */
    ComparatorGen3 &withInterruptPriority(uint8_t priority) {
        config.interrupt_priority = priority;
        return *this;
    }

    /**
     * @brief Whether to enable interrupts in init(). Default is true
     */
    ComparatorGen3 &withEnableOnInit(bool enable) {
        enableOnInit = enable;
        return *this;
    }

    /**
     * @brief Specify the callback function to call
     * 
     * It must have the prototype:
     * 
     * ```
     * void myCallback(nrf_lpcomp_event_t event)
     * ```
     * 
     * Event will be NRF_LPCOMP_EVENT_UP, NRF_LPCOMP_EVENT_DOWN, NRF_LPCOMP_EVENT_CROSS.
     * If you specify interruptMode CHANGE, you only get CROSS, not an indication of
     * which direction you are crossing!
     * 
     * The callback is an ISR, so you must not allocate memory, lock a mutex, or perform
     * lengthy operations from it. You must not use delay(), the Serial, Particle, or Log
     * classes, among others.
     * 
     * The parameter is a C++11 lambda, so you can specify your callback as a lambda
     * instead of a C function pointer. This allows the callback to be a C++ class member
     * instead of a plain C function.
     */
    ComparatorGen3 &withEventCallback(std::function<void(nrf_lpcomp_event_t event)> eventCallback) {
        this->eventCallback = eventCallback;
        return *this;
    }

    /**
     * @brief Initialize the LPCOMP module
     * 
     * You must call the withXXX() methods before calling init()!
     */
    nrfx_err_t init();

    /**
     * @brief Enable the LPCOMP interrupts
     * 
     * By default, they are automatically enabled on init() unless you call 
     */
    void enable();

    /**
     * @brief Disable the LPCOMP interrupts
     */
    void disable();

    /**
     * @brief Disable interrupts and release the LPCOMP resources
     */
    void uninit();


protected:
    /**
     * @brief The constructor is protected because the class is a singleton
     * 
     * Use ComparatorGen3::instance() to instantiate the singleton.
     */
    ComparatorGen3();

    /**
     * @brief The destructor is protected because the class is a singleton and cannot be deleted
     */
    virtual ~ComparatorGen3();

    /**
     * This class is a singleton and cannot be copied
     */
    ComparatorGen3(const ComparatorGen3 &) = delete;

    /**
     * This class is a singleton and cannot be copied
     */
    ComparatorGen3 &operator=(const ComparatorGen3 &) = delete;

    /**
     * @brief Passed to the nRF52 SDK init function
     */
    static void eventHandler(nrf_lpcomp_event_t event);

    /** 
     * @brief Configuration settings for LPCOMP
     * 
     * Fields:
     * nrf_lpcomp_config_t 	hal
     * nrf_lpcomp_input_t 	input
     * uint8_t 	interrupt_priority
     * 
     * nrf_lpcomp_config_t (hal) contains:
     * nrf_lpcomp_ref_t 	reference
     * nrf_lpcomp_detect_t 	detection
     * nrf_lpcomp_hysteresis_t     hyst;
     * 
     * nrf_lpcomp_ref_t (hal.reference):
     * NRF_LPCOMP_REF_SUPPLY_1_8 - Use supply with a 1/8 prescaler as reference.
     * NRF_LPCOMP_REF_SUPPLY_2_8 - Use supply with a 2/8 prescaler as reference.
     * NRF_LPCOMP_REF_SUPPLY_3_8 - Use supply with a 3/8 prescaler as reference.
     * NRF_LPCOMP_REF_SUPPLY_4_8 - Use supply with a 4/8 prescaler as reference.
     * NRF_LPCOMP_REF_SUPPLY_5_8 - Use supply with a 5/8 prescaler as reference.
     * NRF_LPCOMP_REF_SUPPLY_6_8 - Use supply with a 6/8 prescaler as reference. 
     * NRF_LPCOMP_REF_SUPPLY_7_8 - Use supply with a 7/8 prescaler as reference.
     * NRF_LPCOMP_REF_EXT_REF0 - External reference 0.
     * NRF_LPCOMP_CONFIG_REF_EXT_REF1 - External reference 1.
     * 
     * nrf_lpcomp_detect_t (hal.detection):
     * NRF_LPCOMP_DETECT_CROSS - Generate ANADETEC on crossing, both upwards and downwards crossing.
     * NRF_LPCOMP_DETECT_UP - Generate ANADETEC on upwards crossing only.
     * NRF_LPCOMP_DETECT_DOWN - Generate ANADETEC on downwards crossing only.
     * 
     * nrf_lpcomp_hysteresis_t (hal.hyst):
     * NRF_LPCOMP_HYST_NOHYST - Comparator hysteresis disabled.
     * NRF_LPCOMP_HYST_50mV - Comparator hysteresis enabled (typ. 50 mV)
     * 
     * nrf_lpcomp_input_t (input): (Note: nRF input numbers, not Particle pin numbers!)
     * NRF_LPCOMP_INPUT_0 - Input 0.
     * NRF_LPCOMP_INPUT_1 - Input 1.
     * ...
     * NRF_LPCOMP_INPUT_7 - Input 7.
     */
    nrfx_lpcomp_config_t config = NRFX_LPCOMP_DEFAULT_CONFIG;

    /**
     * @brief Whether to automatically call enable() after init (default: true)
     */
    bool enableOnInit = true;

    /**
     * @brief Callback function
     */
    std::function<void(nrf_lpcomp_event_t event)> eventCallback = 0;

    /**
     * @brief Converts a Particle pin number (A1, for example) to a nrf_lpcomp_input_t
     */
    nrf_lpcomp_input_t particlePinToNRF(pin_t pin) const;


    /**
     * @brief Singleton instance of this class
     * 
     * The object pointer to this class is stored here. It's NULL at system boot.
     */
    static ComparatorGen3 *_instance;
};

#endif /* __COMPARATORGEN3_RK_H */
