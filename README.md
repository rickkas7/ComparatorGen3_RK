# ComparatorGen3_RK

*Support for the nRF52 hardware comparator for Particle user firmware*

The nRF52840 MCU in Particle Gen 3 devices (Argon, Boron, B Series SoM, Tracker SoM, Tracker One) has a single analog comparator in the MCU that can be used on any of the available analog input pins. It can detect crossings, rising, falling, or both, optionally with hysteresis. 

Note: The nRF52840 supports both COMP (comparator) and LPCOMP (low-power comparator), but only one can be used at a time. 

| Feature | COMP | LPCOMP |
| :--- | :---: | :---: |
| Differential analog input | &check; | &nbsp; |
| Single-Ended analog input | &check; | &check; |
| External reference voltage | AIN0 or AIN1 | AIN0 - AIN7 |
| Internal reference voltage | 1.2V, 1.8V, or 2.4V | 15 steps of VDD (0.22V each) |

The peripherals are similar but the features of the LPCOMP are what I needed and this library only supports LPCOMP. It wouldn't be hard to modify it to use COMP, but it does not currently work with COMP.

This library uses the nRF52 SDK to access the hardware. The SDK is available to user applications with no additional libraries, however there are some steps that need to be done when using interrupts from user firmware. The following Nordic resources will be helpful:

- [nRF52840 LPCOMP hardware documentation](https://infocenter.nordicsemi.com/topic/ps_nrf52840/lpcomp.html?cp=4_0_0_5_11)
- [nRF52 SDK LPCOMP driver](https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v15.3.0/group__nrfx__lpcomp.html) containing the nRF52 SDK API
- [nRF52 SDK LPCOMP HAL](https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v15.3.0/group__nrf__lpcomp__hal.html) containing the constants and structures you will need

## Usage

See the examples for more information.

- 1-simple - toggles the blue D7 LED on change
- 2-rising - counts the number of rising crossings and prints the results every second

Even though the comparator is fast, interrupt latency makes it impossible to measure really high frequency signals. It sorts of works up to around 100,000 interrupts per second, but it loses some interrupts at the upper end of that, so it won't be particularly reliable at that rate.


If you use an external reference voltage, it must be on A0. It can't be on other pins. If using the internal reference, the possible values are:

- `REF_0V4` (approximately 0.4V)
- `REF_0V8` 
- `REF_1V2`
- `REF_1V7`
- `REF_2V1`
- `REF_2V5`
- `REF_2V9` (approximately 2.9V)
- `PIN_A0`

See the ComparatorGen3_RK.h file. There are numerous comments that explain the functions and values.


## Version History

### 0.0.1 (2021-10-20)

- Initial version
