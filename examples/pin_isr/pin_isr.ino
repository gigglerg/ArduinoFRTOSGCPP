/**
 * Example use of FRTOSGCPP library - Hook a pin change IRQ to a class specific ISR and output via serial a message on changes
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

#define HW_UART      Serial1

#elif defined(ARDUINO_ARCH_STM32)

// Maple Mini FRTOS (Cortex M3)
#include <MapleFreeRTOS900.h>

#define HW_UART      Serial1

#elif defined(ARDUINO_ARCH_AVR)

// AVR8 (UNO/Nano etc.)
#include <FreeRTOS_AVR.h>

#define HW_UART      Serial

#else
#error Unsupported core
#endif


#include <frtosgcpp.h>


/*
 * A class to monitor a UART receive queue, accepting data on when specific text received.  Forwards message to TX UART queue
 *
 * \tparam pinN Pin number of LED output
 * \tparam lineLengthN Text line maximum length (characters, including NULL)
 */
template <uint32_t pinN, uint32_t lineLengthN>
class cPIN : public cTask, protected cIrqHandler {

  public:
    /*
     * Constructor.  make stable instance
     *
     * \param[in] xTxUart Transmit UART
     * \param[in] event Event for observer pattern (optional)
     */
    cPIN(cUARTTX<lineLengthN> &xTxUart, const uint32_t event = 0UL) : _xTxUart(xTxUart) {
    }

    /*
     * Create task and start it + hook port change interrupt for pin
     *
     * \param[in] ulPriority Task priority level.  Default tskIDLE_PRIORITY + 1
     * \param[in] ulStackSize Stack size in Bytes, default configMINIMAL_STACK_SIZE
     * \return Overall state
     * \retval true Join successful
     * \retval false Join failed, one or more problems caused failure
     */
    bool join(const UBaseType_t ulPriority = tskIDLE_PRIORITY + 1, const uint32_t ulStackSize = configMINIMAL_STACK_SIZE) {
      if (!isValidHandle()) {
        start(NULL, tskIDLE_PRIORITY + 1);  // Just above Idle tasking

        // Attach interrupt
        nSupport::cIrqMonitor::attach(static_cast<uint32_t>(pinN), static_cast<cIrqHandler&>(*this), cIrqMonitor::ePINMODE_CHANGE);
      }

      return isValidHandle();
    }

  protected:
    /*
     * Task/thread run handler method, invoked by FRTOS, endless loop.  Handle responses to pin change (not in ISR)
     */
    void run() {
      for (;;) {
        // Wait forever on ISR
        ::ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // It's happened do something...
        _xTxUart.transmit("Change");
      }
    }


    /*
     * ISR for pin
     */
    void isr(const uint32_t pin) {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;

      ::vTaskNotifyGiveFromISR( getHandle(), &xHigherPriorityTaskWoken );
#if !defined(ARDUINO_ARCH_AVR)
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif
    }

  private:
    cUARTTX<lineLengthN> &_xTxUart;   // TX UART instance for output
};

#define UART_LINE_LEN   16

// Transmit UART task and queue to 4 lines
cUARTTX<UART_LINE_LEN>    xUartTX(HW_UART, 4);

// Pin 14 change handler instance, update pin number if you want to monitor another
cPIN<14, UART_LINE_LEN> xPin(xUartTX);


void setup() {
  // Arduino hardware serial setup
  Serial.begin(115200);
  Serial.println("Started");

  xUartTX.join();
  xPin.join();

  // In debug console look for messages when pin changed

  // Start pre-emption (all ok we block here)
  vTaskStartScheduler();
  for (;;);    // dont enter Arduino loop() function...
}

void loop() {
  // We never get here...
}