#include <Arduino.h>
#include <Servo.h>
// #include <vector>
#include <ServoMotto.h>
#include <LDR.h>
#include <SeekModel.h>

using SeekModelSpace::SeekModel;

const auto DEFAULT_SERVO_SPEED = 70;


void initialFrameAlignmentiSetup();
SeekModel initialBaseSetupAlignment(int startAngle = 0, int stopAngle = 180);
SeekModel initialArmAlignmentSetup(int startAngle, int stopAngle);

void trackHorizontalAxis(int offsetAngle, int minDifference = 0, int maxDifference = 30);
void trackVerticalAxis(int offsetAngle, int minDifference = 0, int maxDifference = 30);

// std::vector<SeekModelSpace::SeekModel> pan(ServoMottoSpace::ServoMotto& servo);
// SeekModelSpace::SeekModel avgSeekPan(const std::vector<SeekModelSpace::SeekModel>& seekPan);

LDRSpace::LDR topLeftLDR = LDRSpace::LDR(A0);
LDRSpace::LDR topRightLDR = LDRSpace::LDR(A1);
LDRSpace::LDR bottomLDR = LDRSpace::LDR(A2);

ServoMottoSpace::ServoMotto* baseServoMotto{nullptr};
ServoMottoSpace::ServoMotto* armServoMotto{nullptr};



void setup() {
  
  Serial.begin(9600);
  baseServoMotto =  new ServoMottoSpace::ServoMotto(5, 90);
  delay(300);
  armServoMotto =  new ServoMottoSpace::ServoMotto(6, 45);
  delay(500);

  initialFrameAlignmentiSetup();
}

void loop() {

  

  trackHorizontalAxis(2, 0, 50);
  trackVerticalAxis(3, 0, 50);
  
  delay(400);
}

void initialFrameAlignmentiSetup(){
  auto baseSeek = initialBaseSetupAlignment(0, 180);
  auto baseAngle = baseSeek.angle;
  
  // baseServoMotto->moveTo(baseAngle, DEFAULT_SERVO_SPEED);
  if(baseAngle>180){
    baseServoMotto->moveTo(baseAngle - 180, DEFAULT_SERVO_SPEED);
  }else{
    armServoMotto->moveTo(180 - armServoMotto->currentAngle(), DEFAULT_SERVO_SPEED);
    baseServoMotto->moveTo(baseAngle, DEFAULT_SERVO_SPEED);
  }

  delay(1000);
  

  auto armSeek = initialArmAlignmentSetup(0, 180);
  armServoMotto->moveTo(armSeek.angle, DEFAULT_SERVO_SPEED);
  
}


void trackHorizontalAxis(int offsetAngle, int minDifference, int maxDifference){
  
  auto isPanFlipped = armServoMotto->currentAngle() > 90;



  int diff = isPanFlipped ? topLeftLDR.read() - topRightLDR.read() : topRightLDR.read() - topLeftLDR.read() ;
  auto direction = (diff > 0 ? +1 : -1);
  auto absDiff = direction * diff;
  

  if(absDiff > maxDifference){
    auto initialAngle = baseServoMotto->currentAngle();

    auto angle = initialAngle + (direction * offsetAngle);

    auto shouldFlipPanel = (angle < 0 || angle > 180);

    if(shouldFlipPanel){
      auto repositionAngle =  (angle > 180 ) ? 0 : (angle < 0 ? 180 : angle);

      // Flip the plane to arign to the origin angle
      armServoMotto->moveTo(180 - armServoMotto->currentAngle(), 200);
      baseServoMotto->moveTo(repositionAngle, 100);
    }else {
      auto resolvedToEnd = true;
      baseServoMotto->moveTo(angle, DEFAULT_SERVO_SPEED, resolvedToEnd);
    }
    
  }

}

void trackVerticalAxis(int offsetAngle, int minDifference, int maxDifference){

  

  int count = (topLeftLDR.read() > 0 ? 1 : 0) + (topRightLDR.read() > 0 ? 1 : 0);
  int sum = (topLeftLDR.read() > 0 ? topLeftLDR.read() : 0) + (topRightLDR.read() > 0 ? topRightLDR.read() : 0);
  auto topAvg=  count > 0 ? static_cast<double>(sum) / count : 0;

  auto diff = topAvg - bottomLDR.read();
  auto direction = diff > 0 ? +1 : -1;
  auto absDiff = direction * diff;

  

  if(absDiff > maxDifference){
    auto initialAngle = armServoMotto->currentAngle();
    auto angle = initialAngle + (direction * offsetAngle);
    auto resolvedToEnd = true;
    armServoMotto->moveTo(angle, DEFAULT_SERVO_SPEED, resolvedToEnd);
  }

}


SeekModel initialArmAlignmentSetup(int startAngle, int stopAngle){

  auto seekModel = SeekModel{.angle = -1};

  armServoMotto->moveTo(startAngle, DEFAULT_SERVO_SPEED);

  for(int i{startAngle}; i<stopAngle; ++i){
    armServoMotto->moveTo(i, 50);
    auto topLeftValue = topLeftLDR.read();
    auto topRightValue = topRightLDR.read();
    auto bottomValue = bottomLDR.read();

    auto tempSeekModel = SeekModelSpace::SeekModel{.angle=i, .topLeftValue = topLeftValue, .topRightValue = topRightValue, .bottomValue = bottomValue};
    if(tempSeekModel.avgNonZeroLDR()>seekModel.avgNonZeroLDR()){
      seekModel = tempSeekModel;
    }

  }

  return seekModel;
}


SeekModel initialBaseSetupAlignment(int startAngle, int stopAngle){

  auto seekModel = SeekModel{.angle = -1};

  baseServoMotto->moveTo(startAngle, DEFAULT_SERVO_SPEED);


  // Move half a circle
  for(int i{startAngle}; i<=stopAngle; ++i){
    baseServoMotto->moveTo(i, DEFAULT_SERVO_SPEED);
    auto topLeftValue = topLeftLDR.read();
    auto topRightValue = topRightLDR.read();
    auto bottomValue = bottomLDR.read();
    

    auto tempSeekModel = SeekModelSpace::SeekModel{.angle=i, .topLeftValue = topLeftValue, .topRightValue = topRightValue, .bottomValue = bottomValue};
    if(tempSeekModel.avgNonZeroLDR()>seekModel.avgNonZeroLDR()){
      seekModel = tempSeekModel;
    }

  }



  // Flip the plane to arign to the origin angle
  baseServoMotto->moveTo(startAngle, 200);
  delay(500);
  armServoMotto->moveTo(180 - armServoMotto->currentAngle(), 200);
  
  
  // Revolve remaining half a circle
  for(int i{startAngle}; i<stopAngle; ++i){
    auto angle = i + 1;
    baseServoMotto->moveTo(angle, DEFAULT_SERVO_SPEED);
    auto topLeftValue = topLeftLDR.read();
    auto topRightValue = topRightLDR.read();
    auto bottomValue = bottomLDR.read();

    auto tempSeekModel = SeekModelSpace::SeekModel{.angle=180+angle, .topLeftValue = topLeftValue, .topRightValue = topRightValue, .bottomValue = bottomValue};
    if(tempSeekModel.avgNonZeroLDR()>seekModel.avgNonZeroLDR()){
      seekModel = tempSeekModel;
    }

  }

  return seekModel;

}