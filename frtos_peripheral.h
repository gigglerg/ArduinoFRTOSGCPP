/**
 * \file
 * Microcontroller peripheral interface classes for FRTOS
 * PROJECT          : FRTOS GCPP
 * TARGET SYSTEM    : Arduino, Maple Mini
 */

#ifndef frtosperipheral_h
#define frtosperipheral_h

#include "text.h"
#include "frtos_ext.h"

namespace nFRTOSPeripheral {
    /**
     * A class to read complete text lines that is FRTOS task friendly sourced from Arduino hardware UARTs built upon observer design pattern and queues
     *
     * \tparam N Text line length (characters, including NULL)
     */
    template <uint16_t N>
    class cUARTRX : public nFRTOSExt::cObservedTask, public nText::cTexter<N> {
        public:
            /**
             * Constructor.  Make stable instance
             *
             * \param[in] xSerial Reference to Ardiuno hardware serial port instance, used to receive data
             * \param[in] ucRXDelay FRTOS task delay in ticks between character reading (efficiency aid, default 5)
             */
            cUARTRX(HardwareSerial &xSerial, uint8_t ucRXDelay=5) : _xSerial(xSerial), _ucRXDelay(ucRXDelay) { }


            /**
             * Create task and start it
             *
             * \note Originally setup task name as "UArtR<rx pin>" but no debugger for Arduino code so no point
             * \param[in] priority Task priority level, default +1 above idle
             * \param[in] stackSize Task stack size (Bytes), default 3 * configMINIMAL_STACK_SIZE
             * \return Join state
             */
            bool join(const UBaseType_t priority = tskIDLE_PRIORITY + 1, const uint32_t stackSize=configMINIMAL_STACK_SIZE * 3) {
                if (!isValidHandle()) {
                    start(NULL, priority, stackSize);
                }

                return isValidHandle();
            }


        protected:
            /**
             * Implemented to complete \ref nText::cTexter interface, not used in this here due to nature of simplex class (i.e. receive only)
             *
             *    \param scChar ignore
             */
            void characterWrite(const char scChar) {
                (void)scChar;
            }


            /**
             * For \ref nText::cTexter interface, FRTOS inter-character delay.  Only bothered with this because interrupts are hidden from user for Arduino UART
             */
            void characterReadDelay() const {
                // any character reading delay?  allows other tasks to do stuff.  since lower level serial has good character buffering its a good idea to use it
                if (_ucRXDelay) {
                    vTaskDelay(_ucRXDelay);
                }
            }


            /**
             * For \ref nText::cTexter interface.  Read character from UART
             *
             * \param[out] pscChar Pointer to character receive buffer
             * \return Character read valid state.  When true pscChar is valid otherwise ignore
             */
            bool characterRead(char *pscChar) {
                bool bValid=false;

                if (_xSerial.available()) {
                    *pscChar=_xSerial.read();
                    bValid=true;
                }

                return bValid;
            }


            /**
             * Receive task loop.  Read a line and notify any listeners
             */
            void run() {
                for (;;) {
                    nText::cTexter<N>::blockingReadLine(this, nText::cTexter<N>::_scLine);
                    notify();
                }
            }

        protected:
            uint8_t                _ucRXDelay;
            HardwareSerial&        _xSerial;
    }; // class cUARTRX


    /**
     * A wrapper class for an Arduino hardware UART TX operation that is FRTOS task friendly using queues
     *
     * \tparam N Text line length (characters, including NULL)
     */
    template <uint16_t N>
    class cUARTTX : public nFRTOS::cTask {
        public:
            /**
             * Constructor.  Make stable instance
             *
             * \param[in] xSerial Reference to Ardiuno hardware serial port instance, used to receive data
             * \param ucQueueSize in cTextLine objects
             */
            cUARTTX(HardwareSerial &xSerial, const uint8_t ucQueueSize) : _xSerial(xSerial), _xTxQueue(ucQueueSize) {
            }


            /**
             * Transmit given cTextLine over UART, FRTOS task safe, posts on TX queue
             *
             * \param[in] xTextLine Reference to instance of text line to send.  Should include line ending like "\r\n"
             * \return Transmit success or failure
             */
            bool transmit(nText::cTextLine<N> &xTextLine) {
                return _xTxQueue.send(xTextLine);
            }


            /**
             * Transmit given character string over UART, FRTOS task safe, posts on TX queue
             *
             *    \param[in] pscTextLine Null terminated character string pointer.  Should include line ending "\r\n"
             *    \return Transmit success or failure
             */
            bool transmit(const char *pscTextLine) {
                nText::cTextLine<N> xTextLine(pscTextLine);
    
                return _xTxQueue.send(xTextLine);
            }


            /**
             * Transmit given fixed length character string over UART, FRTOS task safe, posts on TX queue
             *
             * \param[in] pscTextLine Null terminated character string pointer.  Should include line ending "\r\n"
             * \param ucLength Length of string in characters, should not include NULL terminator
             * \return Transmit success or failure
             */
            bool transmit(const char *pscTextLine, const uint8_t ucLength) {
                nText::cTextLine<N> xTextLine(pscTextLine, ucLength);

                return _xTxQueue.send(xTextLine);
            }


            /**
             * Create task and start it + create queue
             *
             * \note Originally setup task name as "UArtT<tx pin>" but no debugger for arduino code so no point
             * \return Join and queue state
             */
            bool join(const UBaseType_t priority = tskIDLE_PRIORITY + 1, const uint32_t stackSize=configMINIMAL_STACK_SIZE * 4) {
                if (!isValidHandle()) {
                    start(NULL, priority, stackSize);

                    _xTxQueue.create();
                }

                return isValidHandle() && _xTxQueue.isValidHandle();
            }

        protected:

            /**
             * Transmit task loop.  Read TX queue and output line over hardware UART
             */
            void run() {
                nText::cTextLine<N> data;

                for (;;) {
                    // Wait for tx data (endlessly), is it ok?
                    if (_xTxQueue.receive(data)) {
                        uint8_t l=data.getLineLength();
                        const char *pscLine=data.getLine();

                        // Send over serial
                        for(uint8_t i=0;i<l;i++) {
                            _xSerial.write(pscLine[i]);
                        }
                    }
                }
            }

        protected:
            nFRTOS::cQueue<nText::cTextLine<N>>    _xTxQueue;
            HardwareSerial&                        _xSerial;
    }; // class cUARTTX
} // namespace nFRTOSPeripheral

#endif // frtosperipheral_h
