/**
 * \file
 * Main include for simple class wrappers and associated structures for interaction with FRTOS.  If you're lazy and 
 * don't want to include specific headers just include this one.
 * PROJECT          : FRTOS GCPP
 * TARGET SYSTEM    : Arduino, Maple Mini
 */

#ifndef frtosgcpp_h
#define frtosgcpp_h

#include "frtos.h"
#include "frtos_ext.h"
#include "frtos_peripheral.h"
#include "pattern.h"
#include "string_helper.h"
#include "support.h"
#include "text.h"

#if !defined(FTROS_GCPP_NONS)
using namespace nFRTOS;
using namespace nFRTOSExt;
using namespace nFRTOSPeripheral;
using namespace nPattern;
using namespace nText;
using namespace nSupport;
#endif

#endif // frtosgcpp_h
