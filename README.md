# FRTOSGCPP

**FRTOSGCPP** library is a light weight collection of C++ classes written for Ardunio as wrappers for Arduino's version(s) of [Free Real Time Operating System (FRTOS)](https://www.freertos.org/).

As FRTOS develops this library should change to include new features and functionality of the C API, wrapped as simple objects with helpers.

These are NOT specific to a chip architecture so should be available for more than AVR.  At present I have tested AVR8, AT91SAM and STM32F103.

[License: GPL2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)


## Using the library

For now as this is a library, Doxygen documentation is available as follows:

* [All documentation](https://gigglerg.github.io/ArduinoFRTOSGCPP/release_100/avr8_html/index.htm)

To rebuild simple use make from within the "./docs" folder.  Use "make clean" to tidy.  Doxygen 1.8.15 is required without DOT.


## Requirements

* Supports
  * Arduino ([AVR8 Mega single flash bank](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)) devices
    * [Arduino Uno/Nano](https://store.arduino.cc/arduino-uno-rev3)
    * [Maple Mini](http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/latest/hardware/maple-mini.html)
    * [Blue Pill](https://medium.com/swlh/super-blue-pill-like-stm32-blue-pill-but-better-6d341d9347da)

  * Arduino ([Due AT91SAM](https://store.arduino.cc/due))
    
  * [STM32 Cortex M series](https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html)
    * Requires [Arduino_STM32](https://github.com/rogerclarkmelbourne/Arduino_STM32)
    * OR
    * Requires [STM32dunio Arduino Core STM32](https://github.com/stm32duino/Arduino_Core_STM32), tested release 1.4.0
      * [My 1.4.0 Windoze patch](https://github.com/gigglerg/Arduino_Core_STM32_Maple_Support) that makes STM32dunio far more usable


## Library Installation

FRTOSGCPP is available for installation in the [Arduino Library Manager].
Alternatively it can be downloaded from GitHub using the following:

1. Download the ZIP file (pick a release branch) from https://github.com/gigglerg/ArduinoFRTOSGCPP
2. Rename the downloaded file to `FRTOSGCPP.zip`
3. From the Arduino IDE: Sketch -> Include Library... -> Add .ZIP Library...
4. Restart the Arduino IDE to see "FRTOSGCPP" library with examples

See the comments in the example sketches for coding use details.


### Include library after your flavour of FRTOS (google it)

```cpp
// Include FRTOS for AVR
#include <FreeRTOS_AVR.h>

// Include FRTOSGCPP headers after FRTOS includes...
#include <frtosgcpp.h>
```

The above includes everything but if you want to be specific, look at the source and include individual headers as desired.  You will have to install your 
chips FRTOS, probably as another library separately.  Look at the few examples for ideas and further information.


## TODOs

* 1).
Apart from refactoring to include std lib when available for Arduino or perhaps Arduino streams.
* 2).
I am considering migrating over to [Google's code style](https://google.github.io/styleguide/cppguide.html) where possible to keep the codebase consistent.


## Gotchas

None yet.


## Contributing

Contributions to the code are welcome using the normal Github pull request workflow.

Please remember the small memory footprint of most micro controllers like Arduino, so try and be efficient when adding new features.


### Code Style

When making contributions, please follow [FRTOS's code style](https://www.freertos.org/FreeRTOS-Coding-Standard-and-Style-Guide.html) where possible to keep 
the codebase consistent.  I am considering switching to Google as Hungarian notation isn't everyones cup of tea and more people seem to hate it than like it.

* Indent *4 spaces*, No hard tabs
* EOL Unix (Library code)
* The rest is online in FRTOS style guide, obviously adapted for C++ (eeek).  Copy and paste where possible
