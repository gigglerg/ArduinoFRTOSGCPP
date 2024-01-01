/**
 * Example use of FRTOSGCPP library - Simple blinky LED
 *
 * Architecture support:
 *  AVR8 (Uno/Nano)
 *  AT91 (Due)
 *  STM32 (F103.  Blue pill/Maple mini)
 *
 * DG, 2019
 */

// Include FRTOS
#if defined(ARDUINO_SAM_DUE)
// Due FRTOS (ARM7)
#include <FreeRTOS_ARM.h>

// Macro to include Ardunio Due FRTOS specifics
//#define FRTOS_SAM_CONTROL

#elif defined(ARDUINO_ARCH_STM32)

// Maple Mini FRTOS (Cortex M3)
#include <MapleFreeRTOS900.h>

#elif defined(ARDUINO_ARCH_AVR)

// AVR8 (UNO/Nano etc.)
#include <FreeRTOS_AVR.h>

#else
#error Unsupported core
#endif


// Include FRTOSGCPP headers after FRTOS includes...
#include <frtosgcpp.h>


/*
 * A class representing a simple operational flashing LED
 *
 * \tparam pinN Pin number of LED output
 */
template <uint32_t pinN>
class cLED : public nFRTOS::cTask {

    public:
        /*
         * Make stable instance
         *
         * \param ulLowTimeout timeout period for LOW pin state.  Default 500 ticks
         * \param ulHighTimeout timeout period for HIGH pin state.  Default 500 ticks
         */
        cLED(uint32_t ulLowTimeout = pdMS_TO_TICKS(500), uint32_t ulHighTimeout = pdMS_TO_TICKS(500)) : _ulTimeout { ulLowTimeout, ulHighTimeout } {
        }

        /*
         * Create task and start it
         *
         * \param[in] ulPriority Task priority level.  Default tskIDLE_PRIORITY + 1
         * \param[in] ulStackSize Stack size in Bytes, default configMINIMAL_STACK_SIZE
         * \return Creation State
         */
        bool join(const UBaseType_t ulPriority = tskIDLE_PRIORITY + 1, const uint32_t ulStackSize=configMINIMAL_STACK_SIZE) {
            start(NULL, ulPriority, ulStackSize);

            return isValidHandle();
        }

    protected:
        /*
         * Task/thread run handler method, invoked by FRTOS, endless loop
         */
        void run() {
            for (;;) {
                digitalWrite(pinN, LOW);
                vTaskDelay(_ulTimeout[0]);
                digitalWrite(pinN, HIGH);
                vTaskDelay(_ulTimeout[1]);
            }
        }

    private:
        uint32_t    _ulTimeout[2];    /// Timeouts for low/high LED states
};


// Assume STM32 is Maple Mini (sorry Blue pill)
#if defined(ARDUINO_ARCH_STM32)

#define MY_BOARD_LED    33

#else

// If your board isnt the default pin13, change...
#define MY_BOARD_LED    13

#endif

cLED<MY_BOARD_LED>    xBoardLED( pdMS_TO_TICKS(500), pdMS_TO_TICKS(250) );

void setup() {
    // Arduino hardware serial setup
    Serial.begin(115200);
    Serial.println("Started");

    xBoardLED.join();

    // Start pre-emption (if all ok we block here)
    vTaskStartScheduler();
}

void loop() {
  // We should never get here, will if tasking fault
}