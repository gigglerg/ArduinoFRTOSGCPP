/**
 * \file
 * Simple class wrappers and associated structures for common design patterns
 * PROJECT          : FRTOS GCPP
 * TARGET SYSTEM    : Arduino, Maple Mini
 */

#ifndef pattern_h
#define pattern_h

#include <string.h>        // i know C api...

namespace nPattern {
    /**
     * A template class implementing Singleton design pattern.  Instantiate like cMyClass &instance=cSingleton<cMyClass>::getInstance();
     *
     * \tparam T Class desired to be made Singleton
     */
    template <class T>
    class cSingleton {
        public:
            /**
             * Get Singleton instance
             *
             * \return Class reference
             */
            static T& getInstance() {
                static T xSingleInstance{};

                return xSingleInstance;
            }


        protected:
            cSingleton() { };                                        ///< Prevent construction
            ~cSingleton() { };                                       ///< Prevent unwanted destruction
            cSingleton(const cSingleton<T>&) = delete;               ///< Prevent construction by copying
            cSingleton& operator=(const cSingleton<T>&) = delete;    ///< Prevent assignment
    }; // class cSingleton


    /**
     * Forward declaration of class cObserver
     */
    typedef class cObserved;


    /**
     * An abstract class performing an implemention of the observer design pattern - observer part (listener)
     */
    class cObserver {
        public:

            /**
             * Update (or callback) method invoked by cObserved objects
             *
             * \param[in] pxSender Pointer to \see cObserved instance, sender of event
             * \return Update handled state
             */
            virtual bool update(const cObserved *pxSender) = 0;

    }; // class cObserver


    /**
     * Maximum number of observed, listeners (observers).  Define your own should you wish to change
     */
#if !defined(COBSERVED_LISTENER_MAX)
    #define COBSERVED_LISTENER_MAX    6        // Arbitrary, finger in the air?  north-east today so 6
#endif


    /**
     * A class performing an implementation of the observer design pattern - subject/observed part (source)
     *
     * \todo Refactor to use template
     */
    class cObserved : public cObserver {
        public:
            cObserved(const uint32_t ulEvent = 0UL) : _ucListenerCount(0), _ulEvent(ulEvent), cObserver() {
                memset(&_pxListener, 0, sizeof(_pxListener));    // NULL pointers
            }


            /**
             * Append an observer instance.  Maximum attachments to \ref COBSERVED_LISTENER_MAX
             *
             * \todo Refactor, attempt to use references
             *
             * \param[in] pxL Pointer to instance
             */
            void appendObserver(cObserver *pxL) {
                if (_ucListenerCount<COBSERVED_LISTENER_MAX) {
                    _pxListener[_ucListenerCount++]=pxL;
                }
            }


            /**
             * Observed instance notifier method, invokes all subscribers.  Each subscriber can accept or reject the notification
             * and the former will stop the invocation process for any given notification.  order of scribers can be important
             */
            void notify() {
                uint8_t ucI;

                // for all observers ... do
                for(ucI=0; ucI<_ucListenerCount; ucI++) {
                    // instance setup?
                    if (_pxListener[ucI]) {
                        // invoke observer
                        if (_pxListener[ucI]->update(this)) {
                            // accepted by this observer and we're done...
                            break;
                        }
                    }
                }
            }


            /**
             * Get event.  The numeric as specified at construction
             *
             * \return Event
             */
            uint32_t getEvent() const {
                return _ulEvent;
            }

        private:
            /**
             * Observered object can also be an observer on another observed object.  In this case we leave it empty
             *
             * \param[in] pxSender Instance of observed or source of the notification, cast accordingly
             * \return Accepted updated state
             * \retval true Accepted a notification so update operation terminates
             * \retval false We never accept any notification, pass it on
             */
            bool update(const cObserved *pxSender) { return false; }     ///< Ignored

        protected:
            uint32_t    _ulEvent;
            uint8_t     _ucListenerCount;
            cObserver*    _pxListener[COBSERVED_LISTENER_MAX];           ///< \todo Make vector
    }; // class cObserved

} // namespace nPattern

#endif // pattern_h
