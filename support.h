/**
 * \file
 * General framework support functionality consider missing from Arduino
 * PROJECT          : FRTOS GCPP
 * TARGET SYSTEM    : Arduino, Maple Mini
 */

#ifndef support_h
#define support_h

#include "pattern.h"

namespace nSupport {
    /**
     * IRQ handler abstract class with user defined ISR method.  The related pin change IRQ when fired will 
     * endup within this class.
     */
    class cIrqHandler {
        public:

            virtual void isr(const uint32_t pin) = 0;

    }; // class cIrqHandler


    /**
     * Maximum I/O pins covered by class \see cIrqMonitor. If you require further I/O covered increase but 
     * also add switch cases to the attach method
     */    
    #define CIRQMONITORED_MAX        24UL


/*! \cond PRIVATE */
    /**
     * Macro to generate pin change ISR handler.  The form is a generic static function which is required by 
     * the Arduino framework as a callback.  From here each ISR checks if its assoicated class handler method 
     * has been defined, if so invoke it.  This is a helper macro to reduce code duplication
     *
     * \param[in] pin Pin number, top less than \ref CIRQMONITORED_MAX
     */    
    #define CIRQMONITORED_PIN_ISR_FUNC(pin)  static void pinISR##pin(void) { \
                                    cIrqHandler* pih = *cIrqMonitor::getHandlerPtr(pin); \
                                    if (pih) { \
                                        pih->isr(pin); \
                                    } \
                                }


    /**
     * Macro to generate a case statement and return of static ISR function related to given pin
     *
     * \param[in] pin Pin number, top less than \ref CIRQMONITORED_MAX
     */    
    #define CIRQMONITORED_PIN_ISR_CASE(pin)    case pin : \
                                        return &pinISR##pin
/*! \endcond */


    /**
     * A class offering IRQ monitoring services that do not require C style callbacks.  The handler 
     * class inherits \ref cIrqHandler which includes a pin specific ISR and is encapsulated within 
     * handler class.
     *
     * \note Maple Mini implementation actually offers something like this with C callbacks but most 
     * other frameworks don't
     */
    class cIrqMonitor {
        public:
            /**
             * Enum of possible pin change modes which are state or transitions
             */
            typedef enum {
                ePINMODE_LOW        = LOW,
                ePINMODE_CHANGE     = CHANGE,
                ePINMODE_RISING     = RISING,
                ePINMODE_FALLING    = FALLING,
                ePINMODE_HIGH       = HIGH,
            }ePINMODE;


            /**
             * Attach ISR to IRQ by pin number
             *
             * \param[in] pin Pin number, top less than \ref CIRQMONITORED_MAX
             * \param[in] handler Reference to handling class instance
             * \param[in] mode IRQ trigger mode /ref ePINMODE
             * \return Attached state
             */
            static bool attach(const uint32_t pin, cIrqHandler &handler, const ePINMODE mode) {
                bool attached = false;
                cIrqHandler** h = cIrqMonitor::getHandlerPtr(pin);

                if ((pin<CIRQMONITORED_MAX) && (!*h)) {
                    attached = true;
                    *h = &handler;
#if defined(_BOARD_MAPLE_MINI_H_)
                    attachInterrupt(static_cast<uint8_t>(pin), cIrqMonitor::getISRPtr(pin), static_cast<ExtIntTriggerMode>(mode));
#else
                    attachInterrupt(digitalPinToInterrupt(pin), cIrqMonitor::getISRPtr(pin), static_cast<uint32_t>(mode));
#endif
                }

                return attached;
            }


            /**
             * Deattach ISR by pin number
             *
             * \param[in] pin Pin number, top less than \ref CIRQMONITORED_MAX
             * \return Deattached state
             */
            static bool deattach(const uint32_t pin) {
                bool deattached = false;
                cIrqHandler** h = cIrqMonitor::getHandlerPtr(pin);

                // cli();
                if ((pin<CIRQMONITORED_MAX) && (*h)) {
                    deattached = true;
                    *h = NULL;
                }
                // sei();

                return deattached;
            } // deattach(...)


            /**
             * Query is an ISR attached by pin number
             *
             * \param[in] pin Pin number, top less than \ref CIRQMONITORED_MAX
             * \return Attached state
             * \retval true attached
             * \retval false deattached
             */
            static bool isAttached(const uint32_t pin) {
                bool attached = false;
                cIrqHandler** h = cIrqMonitor::getHandlerPtr(pin);

                if ((pin<CIRQMONITORED_MAX) && (*h)) {
                    attached = true;
                }

                return attached;
            } // isAttached(...)


            /**
             * Invoke ISR by pin if attached
             *
             * \note Debug aid
             *
             * \param[in] pin Pin number, top less than \ref CIRQMONITORED_MAX
             */
            static void testISR(const uint32_t pin) {
                // cli();
                (void)cIrqMonitor::getISRPtr(pin)();
                // sei();
            }

/*! \cond PRIVATE */
        protected:
            typedef void (*tFp)(void);

            /**
             * Specific pin static ISR function required by Arduino framework.  Invokes handling class ISR if present
             */
            CIRQMONITORED_PIN_ISR_FUNC(0);
            CIRQMONITORED_PIN_ISR_FUNC(1);
            CIRQMONITORED_PIN_ISR_FUNC(2);
            CIRQMONITORED_PIN_ISR_FUNC(3);
            CIRQMONITORED_PIN_ISR_FUNC(4);
            CIRQMONITORED_PIN_ISR_FUNC(5);
            CIRQMONITORED_PIN_ISR_FUNC(6);
            CIRQMONITORED_PIN_ISR_FUNC(7);
            CIRQMONITORED_PIN_ISR_FUNC(8);
            CIRQMONITORED_PIN_ISR_FUNC(9);
            CIRQMONITORED_PIN_ISR_FUNC(10);
            CIRQMONITORED_PIN_ISR_FUNC(11);
            CIRQMONITORED_PIN_ISR_FUNC(12);
            CIRQMONITORED_PIN_ISR_FUNC(13);
            CIRQMONITORED_PIN_ISR_FUNC(14);
            CIRQMONITORED_PIN_ISR_FUNC(15);
            CIRQMONITORED_PIN_ISR_FUNC(16);
            CIRQMONITORED_PIN_ISR_FUNC(17);
            CIRQMONITORED_PIN_ISR_FUNC(18);
            CIRQMONITORED_PIN_ISR_FUNC(19);
            CIRQMONITORED_PIN_ISR_FUNC(20);
            CIRQMONITORED_PIN_ISR_FUNC(21);
            CIRQMONITORED_PIN_ISR_FUNC(22);
            CIRQMONITORED_PIN_ISR_FUNC(23);


            /**
             * Get pointer to ISR handler class instance of \ref cIrqHandler by pin number.  These handlers are held in an internal static array
             * as it was easier than instantiate out side of this class.
             *
             * \todo Make a vector
             *
             * \param[in] pin Pin number, top less than \ref CIRQMONITORED_MAX
             * \return IRQ handler pointer to a pointer.  The internal storage for class pointer, caller can update as required
             */
            static cIrqHandler** getHandlerPtr(uint32_t pin) {
                static cIrqHandler* _handler[CIRQMONITORED_MAX];

                return &_handler[pin];
            }


            /**
             * Get pointer static ISR method by pin number
             *
             * \todo Make a vector
             *
             * \param[in] pin Pin number, top less than \ref CIRQMONITORED_MAX
             * \return Function pointer to related ISR method
             */
            static tFp getISRPtr(uint32_t pin) {
                switch(pin) {
                    CIRQMONITORED_PIN_ISR_CASE(1);
                    CIRQMONITORED_PIN_ISR_CASE(2);
                    CIRQMONITORED_PIN_ISR_CASE(3);
                    CIRQMONITORED_PIN_ISR_CASE(4);
                    CIRQMONITORED_PIN_ISR_CASE(5);
                    CIRQMONITORED_PIN_ISR_CASE(6);
                    CIRQMONITORED_PIN_ISR_CASE(7);
                    CIRQMONITORED_PIN_ISR_CASE(8);
                    CIRQMONITORED_PIN_ISR_CASE(9);
                    CIRQMONITORED_PIN_ISR_CASE(10);
                    CIRQMONITORED_PIN_ISR_CASE(11);
                    CIRQMONITORED_PIN_ISR_CASE(12);
                    CIRQMONITORED_PIN_ISR_CASE(13);
                    CIRQMONITORED_PIN_ISR_CASE(14);
                    CIRQMONITORED_PIN_ISR_CASE(15);
                    CIRQMONITORED_PIN_ISR_CASE(16);
                    CIRQMONITORED_PIN_ISR_CASE(17);
                    CIRQMONITORED_PIN_ISR_CASE(18);
                    CIRQMONITORED_PIN_ISR_CASE(19);
                    CIRQMONITORED_PIN_ISR_CASE(20);
                    CIRQMONITORED_PIN_ISR_CASE(21);
                    CIRQMONITORED_PIN_ISR_CASE(22);
                    CIRQMONITORED_PIN_ISR_CASE(23);
                } // switch(pin)

                return &pinISR0;
            } // getISRPtr(...)
/*! \endcond */

    }; // class cIrqMonitor

} // namespace nSupport

#endif // support_h
