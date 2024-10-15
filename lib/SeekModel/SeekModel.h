#ifndef SeekModel_h
#define SeekModel_h

#include <Arduino.h>

namespace SeekModelSpace{
    struct SeekModel{
        int angle{0};
        int topLeftValue = 0;
        int topRightValue = 0;
        int bottomValue = 0;


        double avgLDR(){
            return (topLeftValue + topRightValue + bottomValue) / 3;
        }

        double avgNonZeroLDR(){
            int count = (topLeftValue > 0 ? 1 : 0) + (topRightValue > 0 ? 1 : 0) + (bottomValue > 0 ? 1 : 0);
            int sum = (topLeftValue > 0 ? topLeftValue : 0) + (topRightValue > 0 ? topRightValue : 0) + (bottomValue > 0 ? bottomValue : 0);
            
            return count > 0 ? static_cast<double>((sum) / count) : 0;
        }
    };
}

#endif