/**
 * \file
 * Extension/helper classes for FRTOS
 * PROJECT          : FRTOS GCPP
 * TARGET SYSTEM	: Arduino, Maple Mini
 */

#ifndef frtosext_h
#define frtosext_h

#include "frtos.h"
#include "pattern.h"

namespace nFRTOSExt {
	/**
	 * A class to representing interfaces of observer pattern with a FRTOS task
	 */
	class cObserverTask : public nPattern::cObserver, public nFRTOS::cTask {};


	/**
	 * A class to representing interfaces of observer/observed pattern with a FRTOS task
	 */
	class cObservedTask : public nPattern::cObserved, public nFRTOS::cTask {
		public:
			/**
			 * Constructor.  Make stable instance
             *
             * param[in] ulEvent Event numeric, a value used to distinguish this event.  Default 0
			 */
			cObservedTask(const uint32_t ulEvent = 0UL) : nPattern::cObserved(ulEvent), nFRTOS::cTask() { }
	};
} // namespace nFRTOSExt

#endif // frtosext_h
