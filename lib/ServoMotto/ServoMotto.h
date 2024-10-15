#ifndef ServoMotto_h
#define ServoMotto_h


#include <Arduino.h>
#include <Servo.h>

namespace ServoMottoSpace{
    using namespace std;

    class ServoMotto{
        public:
            ServoMotto(int pin, int initialPosition = 90): mPin{pin}, mServo{Servo()}{
                this->attach(pin);
                this->moveTo(initialPosition, 100);
            }

            ~ServoMotto(){
                // delete mServo;
            }

            bool canOffsetInRange(int offset);
            int currentAngle();

            int resolvedEndAngle(int angle){
                if(inRange(angle)){
                    return angle;
                }else{
                    if(angle > 180){
                        return 180;
                    }else if(angle<0){
                        return 0;
                    }
                }

                return 0;
            }
            
            

            void moveTo(int angle, int speed = 50, bool resolvedToEnd = false){

                auto resolvedAngle = resolvedToEnd ?  resolvedEndAngle(angle) : angle;
                
                if(inRange(resolvedAngle)){
                    
                    if(speed > 0){
                        auto direction = currentAngle() > resolvedAngle ? -1 : 1;
                        auto timeSpeed = 1000 / speed;
                        for(int current{currentAngle()}; current != resolvedAngle; current +=direction){
                            write(current);
                            delay(timeSpeed);
                        }
                    }else{
                        write(resolvedAngle);
                    }
                    
                    
                }
            }

            
            int getAttachedPin() const{
                return mPin;
            }


            int read() const {
                int value = analogRead(getAttachedPin());
                return value;
            }

            void detached() {
                mServo.detach();
            }

            ServoMotto &operator+(const int &value)
            {
                auto currentAngle = this->currentAngle();
                this->writeMicroseconds(currentAngle + value);
                return *this;
            }
            ServoMotto &operator+(const double &value)
            {
                auto currentAngle = this->currentAngle();
                this->writeMicroseconds(currentAngle + value);
                // this->write(currentAngle + value);
                return *this;
            }

            ServoMotto &operator-(const int &value)
            {
                return (*this) + (-value);
            }

            ServoMotto &operator-(const double &value)
            {
                return (*this) + (-value);
            }

            

            bool inRange(int angle)
            {
                return (_minAbsoluteLimit <= angle) && (angle <= _maxAbsoluteLimit);
            }

        private:
            uint8_t attach(const uint8_t &pin, const int &minPulse = MinPulse, const int &maxPulse = MaxPulse);
            static const int MinPulse = 1000;
            static const int MaxPulse = 2000;
            static const int _maxAbsoluteLimit = 180;
            static const int _absoluteMidpoint = 90;
            static const int _minAbsoluteLimit = 0;
            static const int _maxRelativeUpwardLimit = +90;
            static const int _minRelativeUpperLimit = 0;
            static const int _maxRelativeDownwardLimit = 0;
            static const int _minRelativeDownwardLimit = -90;
            static const int _relativeMidpoint = 0;

            void writeMicroseconds(double angle)
            {

                auto lPulse = MinPulse;
                auto hPulse = MaxPulse;

                auto resolvedAngle{lPulse + (angle / 180) * (hPulse - lPulse)};

                this->mServo.writeMicroseconds(resolvedAngle);
            }

            void write(double angle)
            {
                this->mServo.write(angle);
            }

        private:
            int mPin;

        public:
            Servo mServo;
            
    };
}

#endif