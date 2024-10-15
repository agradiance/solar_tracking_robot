#ifndef LDR_h
#define LDR_h

#include <Arduino.h>

namespace LDRSpace{
    class LDR{
        public:
            LDR(int pin): mPin{pin}{}


        int getPin() const{
            return mPin;
        }

        int read() const {
            int value = analogRead(getPin());
            return value;
        }
        private:
            int mPin;
    };
}

#endif