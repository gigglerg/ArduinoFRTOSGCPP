/**
 * Example use of FRTOSGCPP library - Associate a Arduino UART with 2 tasks and queues for RX and TX, then classes implementing 
 * observer pattern, listening for and accepting specific messaging.  When accepted the message is passed to TX UART 
 * queue for transmit.
 *
 * Architecture support:
 *  AVR8 (Uno/Nano)
 *  AT91 (Due)
 *  STM32 (F103.  Blue pill/Maple mini)
 *
 * DG, 2019
 */

#include <limits.h>
#include <string.h>

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


// Macro to stop issuing using on all FTROSGCPP namespaces.  If defined, you specify
//#define FTROS_GCPP_NONS
#include <frtosgcpp.h>


/*
 * A class to monitor a UART receive queue, accepting data on when specific text received.  Forwards message to TX UART queue
 *
 * \tparam lineLengthN Text line maximum length (characters, including NULL)
 * \tparam msgLen Message maximum length (characters, including NULL)
 */
template <uint32_t lineLengthN, uint32_t msgLen>
class cUARTMessage : public nPattern::cObserved {

    public:
        /*
         * Constructor.  make stable instance
         *
         * \param[in] xRxUart Receive UART
         * \param[in] xTxUart Transmit UART
         * \param[in] message Message for instance to search receive UART data
         * \param[in] event Event for observer pattern (optional)
         */
        cUARTMessage(nFRTOSPeripheral::cUARTRX<lineLengthN> &xRxUart, nFRTOSPeripheral::cUARTTX<lineLengthN> &xTxUart, \
                            const char *message, const uint32_t event = 0UL) : _xTxUart(xTxUart) {
            // Add this instance to observer on receive UART
            xRxUart.appendObserver(static_cast<nPattern::cObserver*>(this));
            strcpy(_message, message);
            _messageLength = strlen(_message);
        }

    protected:
        /*
         * Observed UART instance containing possible messaging data.  We won't bother post to our own tasks queue just 
         * handle event in UART's task (i.e. from here)
         *
         * \param pxSender Instance of observed or source of the notification, cast accordingly
         * \return Accept message state
         */
        bool update(const cObserved *pxSender) {
            bool accept = false; // don't accept, will pass to next listener

            // Get our receive uart instance
            const nFRTOSPeripheral::cUARTRX<lineLengthN> *pUart=reinterpret_cast<const nFRTOSPeripheral::cUARTRX<lineLengthN>*>(pSender);

            if (0 == ::strncmp( pUart->getLine(), _message , _messageLength)) {
                accept = true;

                // Output received message as we handle it...  couple of lines so group with critical section
                taskENTER_CRITICAL();
                _xTxUart.transmit("RX: ");
                _xTxUart.transmit(_message);
                taskEXIT_CRITICAL();
            }

            return accept;
        }

    protected:
        uint8_t    _messageLength;                          // Message length (characters)
        char       _message[msgLen];                        // Message to handle
        nFRTOSPeripheral::cUARTTX<lineLengthN> &_xTxUart;   // TX UART instance for output
}; // cUARTMessage


// Receive and Transmit UART tasks and queues
nFRTOSPeripheral::cUARTRX<32>    xUartRX(HW_UART);
nFRTOSPeripheral::cUARTTX<32>    xUartTX(HW_UART, 5);    // TX queue size 5, don't expect more than this many lines buffered between task use

// UART message handlers
cUARTMessage<32, 12>            xUartMsgHello(xUartRX, xUartTX, "HELLO");
cUARTMessage<32, 12>            xUartMsgWorld(xUartRX, xUartTX, "WORLD");


void setup() {
    // Arduino hardware serial setup
    Serial.begin(115200);
    Serial.println("Started");

    xUartRX.join();
    xUartTX.join();

    // In debug console on seperate lines, type "HELLO" or "WORLD" to get responses

    // Start pre-emption (all ok we block here)
    vTaskStartScheduler();
    for(;;);    // dont enter Arduino loop() function...
}

void loop() {
}