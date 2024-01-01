/**
 * \file
 * Simple text handling helper classes
 * \todo    Refactor with Arduino streams
 * PROJECT            : FRTOS GCPP
 * TARGET SYSTEM    : Arduino, Maple Mini
 */

#ifndef text_h
#define text_h

namespace nText {
    /**
     * A class to represent a text line string
     *
     * \todo Refactor as std::string with fixed size unit allocation
     *
     * \tparam N Text line length (characters, including NULL)
     */
    template <uint16_t N>
    class cTextLine {
        public:
            /**
             * Default constructor, make stable instance
             */
            cTextLine() : _ucLength(N) { }


            /**
             * Null terminated string constructor
             *
             * \note Source string will be truncated as required
             * \param[in] pcsData Pointer to source NULL terminated character string
             */
            cTextLine(const char *pcsData) {
                setLine(pcsData, strlen(pcsData));
            }


            /**
             * Null terminated string, text line constructor
             *
             * \note Source string will be truncated as required
             * \param[in] pcsData Pointer to source NULL terminated character string
             * \param[in] ucLength Length of character string
             */
            cTextLine(const char *pcsData, const uint8_t ucLength) {
                setLine(pcsData, ucLength);
            }


            /**
             * Set string text line
             *
             * \param pcsData Pointer to source character string
             * \param ucLength length of character string
             */
            void setLine(const char *pcsData, const uint8_t ucLength) {
                if (ucLength>N-1) {
                    _ucLength=N-1;    // deal with including a NULL
                }else {
                    _ucLength=ucLength;
                }
                for(uint8_t ucI=0;ucI<_ucLength;ucI++) {
                    _scLine[ucI]=pcsData[ucI];
                }
                // last character NULL terminator?
                if (_scLine[_ucLength-1]!=0x00) {
                    // no so install NULL, dont increase buffer length as it is not o/p, only set so system functions like printf("%s", ...) via getLine(...) work
                    _scLine[_ucLength]=0x00;
                }
            }


            /**
             * Get pointer to null terminated string
             *
             * \todo Refactor with copy rather than exposing internal string
             * \attention Only valid when \ref getLineLength >0
             * \return Pointer to null terminated line string
             */
            const char *getLine() const {
                return _scLine;
            }


            /**
             * Get line string length in characters
             *
             * \return Characters N (not including NULL terminator, will include other control characters like newline or carriage return)
             */
            uint8_t getLineLength() const {
                return _ucLength;
            }

        protected:
            char                _scLine[N+1];
            uint8_t             _ucLength;
    }; // class cTextLine


    /**
     * Class to aid device specific text line i/o
     *
     * \tparam N Text line length (characters, including NULL)
     */
    template <uint16_t N>
    class cTexter : public cTextLine<N> {
        public:
            cTexter() : cTextLine<N>() { }

        protected:

            /**
             * Character read delay
             */
            virtual void characterReadDelay() const = 0;


            /**
             * Character read
             *
             *    \return Read status.  Determine if pscChar is valid or not
             */
            virtual bool characterRead(char *pscChar) = 0;


            /**
             * Character write
             */
            virtual void characterWrite(const char scChar) = 0;


            /**
             * Blocking read line as character string from given \ref cTexter instance
             *
             * \note Assumes a line string is terminated by "\r\n" character combo
             * \param pxThis Instance of CTexter (data source)
             * \param pscData Data buffer to store characters
             */
            static void blockingReadLine(cTexter<N> *pxThis, char *pscData) {
                char    scLast=0,scCurrent;
                uint8_t    ucLength=0;

                while(1) {
                    if (pxThis->characterRead(&scCurrent)) {
    
                        if (ucLength>=N) {
                            ucLength=0;
                        }
                        pscData[ucLength++]=scCurrent;

                        if (scCurrent=='\n' && scLast=='\r') {

                            if (ucLength>1) {
                                pscData[ucLength]=0;    // install null terminator and we're done...

                                pxThis->_ucLength=ucLength;
                                break;
                            }
                        }

                        scLast=scCurrent;
                    }

                    pxThis->characterReadDelay();
                }
            }


            /**
             * Blocking write line as character string, NULL terminated
             *
             * \note Assumes a line string includes "\r\n" termination characters and finishes output on NULL character or ucMaxLength
             * \param pxThis Instance of CTexter (data destination)
             * \param pscData Data buffer of source characters
             */
            static void blockingWriteLine(cTexter<N> *pxThis, const char *pscData) {
                char    ucLength=0,scCurrent;

                while(1) {
                    scCurrent=pscData[ucLength++];
                    if (scCurrent==0x00) {
                        break;
                    }
                    pxThis->characterWrite(scCurrent);
                    if (ucLength>=N) {
                        break;
                    }
                }
            }

    }; // class cTexter

} // namespace nText

#endif // text_h
