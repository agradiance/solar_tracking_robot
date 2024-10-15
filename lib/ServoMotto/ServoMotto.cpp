#include <ServoMotto.h>

namespace ServoMottoSpace{

    
    uint8_t ServoMotto::attach(const uint8_t &pin, const int &minPulse, const int &maxPulse)
    {
        return mServo.attach(pin);
        // return mServo.attach(pin, minPulse, maxPulse);
    }

    bool ServoMotto::canOffsetInRange(int offset)
    {
        auto angle{currentAngle() + offset};
        return inRange(angle);
    }

    int ServoMotto::currentAngle()
    {
        return mServo.read();
    }
}