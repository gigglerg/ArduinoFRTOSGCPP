/**
 * \file
 * Simple class wrappers and associated structures for interaction with FRTOS
 * PROJECT          : FRTOS GCPP
 * TARGET SYSTEM    : Arduino, Maple Mini
 */

#ifndef frtos_h
#define frtos_h

#if defined(ARDUINO_SAM_DUE) && defined(FRTOS_SAM_CONTROL)
extern int sysTickEnabled;
#endif // defined(ARDUINO_SAM_DUE) && defined(FRTOS_SAM_CONTROL)

namespace nFRTOS {
    /**
     * An abstract class wrapping a single FRTOS task
     */
    class cTask {
        public:
            /**
             * Default constructor, nake stable instance
             */
            cTask() : _xTHandle(NULL) , _bRunning(false) { }


            /**
             * Get FRTOS task handle or NULL.  \ref start for details
             *
             * \return TaskHandle_t
             */
            TaskHandle_t getHandle() {
                return _xTHandle;
            }


            /**
             * Test FRTOS task handle
             *
             * \return Valid state
             */
            bool isValidHandle() const {
                return (_xTHandle?true:false);
            }


            /**
             * Get FRTOS task running state
             *
             * \return Running state
             */
            bool isRunning() {
                return _bRunning;
            }


            /**
             * Suspend task
             *
             * \param[in] xTHandleSpecific Task hande, default NULL = this task
             */
            void suspend(const TaskHandle_t xTHandleSpecific=NULL) const {
                // specific task?
                if (xTHandleSpecific) {
                    vTaskSuspend(xTHandleSpecific);
                }else {
                    // this task
                    vTaskSuspend(_xTHandle);
                }
            }


            /**
             * Resume task
             *
             * \param[in] xTHandleSpecific Task hande, default NULL = this task
             */
            void resume(const TaskHandle_t xTHandleSpecific=NULL) const {
                // specific task?
                if (xTHandleSpecific) {
                    vTaskResume(xTHandleSpecific);
                }else {
                    // this task
                    vTaskResume(_xTHandle);
                }
            }


#if defined(ARDUINO_SAM_DUE) && defined(FRTOS_SAM_CONTROL)
            /**
             * Tasking stop.  Specific Arduino Atmal91SAM implementation where a global in the framework is used to control interrupt driven task switching.
             * Use this method to disable tasking while still keeping global IRQs enabled.
             *
             * \attention Caller should define FRTOS_SAM_CONTROL if this API is required
             */
            void taskingStop() const {
                sysTickEnabled = 0;
            }


            /**
             * Tasking start.  Specific Arduino Atmal91SAM implementation where a global in the framework is used to control interrupt driven task switching.
             * Use this method to enable tasking while still keeping global IRQs enabled.
             *
             * \attention Caller should define FRTOS_SAM_CONTROL if this API is required
             */
            void taskingStart() const {
                sysTickEnabled = 1;
            }
#endif // defined(ARDUINO_SAM_DUE) && defined(FRTOS_SAM_CONTROL)


        protected:
            /**
             * Create task and start running
             *
             * \note Running state will not change until \see run() invoked
             * \note Use \ref isValidHandle to determine success of task creation
                * \attention Invoke from \see join() implementation
             *
             * \param[in] pcTaskName Pointer to character string for task name (null terminated).  Can be NULL pointer if no string desired
             * \param[in] ulPriority Task priority
             * \param[in] ulStackSize Stack size in Bytes, default configMINIMAL_STACK_SIZE
             */
            void start(const char *pcTaskName, const UBaseType_t ulPriority, const uint32_t ulStackSize=configMINIMAL_STACK_SIZE) {
                xTaskCreate(cTask::taskHandler,
                            pcTaskName,
                            ulStackSize,
                            static_cast<void *>(this),    // pvParameters pointer passed to cTask::taskHandler
                            ulPriority,
                            &_xTHandle);
            }


            /**
             * Create task and start running via \ref start.  Likely create other FRTOS elements as required by implementing class
             *
             * \note Pre-condition, internally invoke \ref start and make public for users
             *
             * \param[in] ulPriority Task priority level.  Default tskIDLE_PRIORITY + 1
             * \param[in] ulStackSize Stack size in Bytes, default configMINIMAL_STACK_SIZE
             * \return Overall state
             * \retval true Join successful
             * \retval false Join failed, one or more problems caused failure
             */
            virtual bool join(const UBaseType_t ulPriority = tskIDLE_PRIORITY + 1, const uint32_t ulStackSize=configMINIMAL_STACK_SIZE) = 0;


            /**
             * Task main loop, implementation should be a non-returning endless loop in most cases
             *
             * \note Keep protected or lower to private
             */
            virtual void run() = 0;


            /**
             * Task callback handler, invokes user defined \see run() method
             *
             * \param[in,out] pvParameters Pointer to this instance
             */
            static void taskHandler(void *pvParameters) {
                // setup?
                if (NULL!=pvParameters) {
                    // change running flag + invoke user method
                    reinterpret_cast<cTask *>(pvParameters)->_bRunning=true;
                    reinterpret_cast<cTask *>(pvParameters)->run();
                    reinterpret_cast<cTask *>(pvParameters)->_bRunning=false;
                }
            }


        protected:
            bool             _bRunning;
            TaskHandle_t     _xTHandle;
    }; // class cTask


    /**
     * A class wrapping a single queue
     *
     * \tparam QType Data type stored in queue
     */
    template<class QType> class cQueue {
        public:
            /**
             * Default constructor.  Make stable instance
             *
             * \param[in] uxQueueLength Size of queue (elements)
             */
            cQueue(UBaseType_t uxQueueLength) : _xQHandle(NULL), _xSize(uxQueueLength) { }


            /**
             * Destructor.  Release queue
             */
            ~cQueue() {
                if (_xQHandle) {
                    vQueueUnregisterQueue(_xQHandle);
                }
            }


            /**
             * Create FRTOS queue and test handle, \ref isValidHandle
             *
             * \return Creation state
             */
            bool create() {
                _xQHandle=xQueueCreate( _xSize, sizeof(QType) );

                return isValidHandle();
            }


            /**
             * Get FRTOS queue handle or NULL
             *
             * \return Queue handle
             */
            QueueHandle_t getHandle() const {
                return _xQHandle;
            }


            /**
             * Test FRTOS queue handle
             *
             * \return Handle valid state
             */
            bool isValidHandle() const {
                return (_xQHandle?true:false);
            }


            /**
             * Get size of data structure carried by queue
             *
             * \return Size (Bytes)
             */
            UBaseType_t getSize() const {
                return _xSize;
            }


            /**
             * Get queue spaces available for send (free space in QType unit counts)
             *
             *    \return Spaces available (elements free)
             */
            UBaseType_t getSpacesAvailable() const {
                return uxQueueSpacesAvailable( _xQHandle );
            }


            /**
             * Peek at data on queue.  If xTicksToWait expires and no data received bReceived will be false
             *
             * \param[out] bReceived Valid element receive status.  If true then returned element can be used
             * \param[in] xTicksToWait Ticks to wait.  Default portMAX_DELAY (unlimited)
             * \return Element
             */
            QType peek(bool &bReceived, const TickType_t xTicksToWait=portMAX_DELAY) const {
                QType xData;

                bReceived=false;
                 if (pdTRUE == xQueuePeek(_xQHandle, static_cast<void *>(xData), xTicksToWait)) {
                    bReceived=true;
                }

                return xData;
            }


            /**
             * Receive data from queue.  If xTicksToWait expires and no data received bReceived will be false
             *
             * \param[out] xData Reference to element received.  If returned state true then this is valid
             * \param[in] xTicksToWait Ticks to wait.  Default portMAX_DELAY (unlimited)
             * \return Receive status
             */
            bool receive(QType &xData, const TickType_t xTicksToWait=portMAX_DELAY) const {
                bool bReceived = false;

                 if (pdTRUE == xQueueReceive(_xQHandle, static_cast<void *>(&xData), xTicksToWait)) {
                    bReceived=true;
                }

                return bReceived;
            }

            /**
             * Send xData to queue.  If xTicksToWait expires and no data sent due to block, result will be false
             *
             * \param[in] xData Reference to element to send
             * \param[in] xTicksToWait Ticks to wait.  Default portMAX_DELAY (unlimited)
             * \return Send status
             */
            bool send(const QType &xData, const TickType_t xTicksToWait=portMAX_DELAY) const {
                bool bSent=false;

                 if (pdTRUE == xQueueSend(_xQHandle, static_cast<const void *>(&xData), xTicksToWait)) {
                    bSent=true;
                }

                return bSent;
            }

        protected:
            UBaseType_t        _xSize;
            QueueHandle_t      _xQHandle;
    }; // class cQueue

} // namespace nFRTOS

#endif // frtos_h
