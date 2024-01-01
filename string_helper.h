/**
 * \file
 * Part of the text handling classes, string helpers
 * \todo    Refactor with Arduino streams
 * PROJECT          : FRTOS GCPP
 * TARGET SYSTEM    : Arduino, Maple Mini
 */

#ifndef stringhelper_h
#define stringhelper_h

namespace nText {
    /**
     * A helper class for strings (character arrays).  sprintf(...) in maple builds adds 20% to builds and becomes unstable, hence these short helpers just for arduino builds
     */
    class cStringHelper {
        public:

            /**
             * Reverse character string with known length
             *
             * \param[in,out] pscStr String pointer
             * \param[in] ucLength Length (characters)
             * \return Character string pointer
             */
            static char *reverse(char *pscStr, uint8_t ucLength) {
                uint8_t ucI;

                if (ucLength<=1) {
                    return pscStr;
                }
                ucLength--;    // last character always -1
                for(ucI=0;ucI<(ucLength+1)>>1;ucI++) {
                    // exclusive OR swap character [ucI] with [ucLength-ucI]
                    pscStr[ucI] = pscStr[ucI] ^ pscStr[ucLength-ucI];
                    pscStr[ucLength-ucI] = pscStr[ucLength-ucI] ^ pscStr[ucI];
                    pscStr[ucI] = pscStr[ucI] ^ pscStr[ucLength-ucI];
                }

                return pscStr;
            } // reverse(...)


            /**
             * String from integer, itoa like function.  tested base 10 and 16
             * source: http://stackoverflow.com/questions/190229/where-is-the-itoa-function-in-linux
             *
             * \param[out] pscStr String pointer
             * \param[in] n Integer
             * \param[in] ucBase Radix
             * \return Character string pointer
             */
            static char *fromInt(char *pscStr, int32_t n, const uint8_t ucBase) {
                static char _scASCII[]={ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
                int32_t i, sign;

                // Record sign
                if ((sign = n) < 0) {
                    n = -n;        // abs(n)
                }

                i = 0;
                do {
                    // Generate digits in reverse order
                    pscStr[i++] = _scASCII[(n % ucBase) & 15];   // Get next digit
                }while ((n /= ucBase) > 0);    // Delete it
                if (sign < 0) {
                    pscStr[i++] = '-';
                }
                pscStr[i] = '\0';    // Null terminator

                return cStringHelper::reverse(pscStr, i);    // Reverse string to complete
            } // fromInt(...)


            /**
             * String from float (takes a double so slightly misleading)
             * source: printFloat for Arduno Maple Mini in core/maple/print.cpp
             * Unfortunately its a private method and within a class that would be bloated for my purposes here, hence is a refactored copy
             *
             * \param[out] pscStr String pointer
             * \param[in] n Integer
             * \param[in] digits Number of digits to resolve
             * \return Character string pointer
             */
            static uint8_t fromFloat(char *pscStr, double n, uint8_t digits) {
                uint8_t ucLength=0;

                // Ignore dealing with super large values - we know they wont be in our working system

                // Handle negative numbers
                if (n < 0.0) {
                    pscStr[ucLength++]='-';
                    n = -n;
                }
                pscStr[ucLength]='\0';

                // Simplistic rounding strategy so that e.g. print(1.999, 2)
                // prints as "2.00"
                double rounding = 0.5;
                for (uint8_t i = 0; i < digits; i++) {
                    rounding /= 10.0;
                }
                n += rounding;

                // Extract the integer part of the number and print it
                uint32_t int_part = (uint32_t)n;
                double remainder = n - int_part;
                cStringHelper::fromInt(&pscStr[ucLength], int_part, 10);
                ucLength=strlen(pscStr);

                // Print the decimal point, but only if there are digits beyond
                if (digits > 0) {
                    pscStr[ucLength++]='.';
                    pscStr[ucLength]='\0';
                }

                // Extract digits from the remainder one at a time
                while(digits-- > 0) {
                    remainder *= 10.0;
                    uint32_t to_print = (uint32_t)remainder;

                    cStringHelper::fromInt(&pscStr[ucLength], to_print, 10);
                    ucLength=strlen(pscStr);

                    remainder -= to_print;
                }

                return ucLength;
            } // fromFloat(...)
    }; // cStringHelper

} // namespace nText

#endif // stringhelper_h
