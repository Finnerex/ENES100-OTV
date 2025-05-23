#ifndef DRIVETRAIN_H
#define DRIVETRAIN_H

#include "Arduino.h"
#include <math.h>
#include "util.h"
#include "Enes100.h"
#include "pins_arduino.h"
#include <Servo.h>  

// Motor pin defines
#define MOTOR_1_PWM_PIN 3
#define MOTOR_2_PWM_PIN 5
#define MOTOR_3_PWM_PIN 6

#define MOTOR_1_DIR_PIN A4
#define MOTOR_2_DIR_PIN 7
#define MOTOR_3_DIR_PIN 2

#define MOTOR_1_DIR_PIN_2 A5
#define MOTOR_2_DIR_PIN_2 A2
#define MOTOR_3_DIR_PIN_2 A3

#define ULTRA_TRIGGER_PIN 10
#define ULTRA_ECHO_PIN 9

#define CLAW_PWM_PIN 11

#define ARM_EXTEND_PIN 12
#define ARM_EXTEND_PIN_2 13

#define VIBRATION_MOTOR_PIN 8

class Otv {
  
  #define ULTRA_DISTANCE_WARNING 20

  // #define CLAW_OPEN_VALUE 0
  // #define CLAW_CLOSED_VALUE 50

  #define MOVE_FORWARD_VALUE 0.0010
  #define MOVE_LEFT_VALUE 0.0010
  #define MOVE_RIGHT_VALUE 0.0010

  // private:
  //   Servo clawMotor;

  public:
    float speed;

  public:
    Otv() {
      pinMode(MOTOR_1_DIR_PIN, OUTPUT);
      pinMode(MOTOR_2_DIR_PIN, OUTPUT);
      pinMode(MOTOR_3_DIR_PIN, OUTPUT);
      pinMode(MOTOR_1_DIR_PIN_2, OUTPUT);
      pinMode(MOTOR_2_DIR_PIN_2, OUTPUT);
      pinMode(MOTOR_3_DIR_PIN_2, OUTPUT);

      pinMode(MOTOR_1_PWM_PIN, OUTPUT);
      pinMode(MOTOR_2_PWM_PIN, OUTPUT);
      pinMode(MOTOR_3_PWM_PIN, OUTPUT);

      pinMode(ULTRA_ECHO_PIN, INPUT);
      pinMode(ULTRA_TRIGGER_PIN, OUTPUT);

      pinMode(CLAW_PWM_PIN, OUTPUT);
      // clawMotor.attach(CLAW_PWM_PIN);

      pinMode(ARM_EXTEND_PIN, OUTPUT);
      pinMode(ARM_EXTEND_PIN_2, OUTPUT);

      pinMode(VIBRATION_MOTOR_PIN, OUTPUT);
    }

  // returns the x, y position of the otv according to the vision system
  Vector2 getPosition() {
    while (!Enes100.isVisible())
      delay(1);

    return { Enes100.getX(), Enes100.getY() };
  }

  // returns the theta rotation of the otv according to the vision system
  float getRotation() {
    while (!Enes100.isVisible())
      delay(1);

    return Enes100.getTheta();
  }

  // returns distance to an object detected by the ultrasonic sensor in cm
  float GetUltraDistance() { 
    // Clears the trigPin
    digitalWrite(ULTRA_TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(ULTRA_TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRA_TRIGGER_PIN, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    float ultraDuration = pulseIn(ULTRA_ECHO_PIN, HIGH);

    // Prints the distance on the Serial Monitor
    // Serial.print("Distance: ");
    // Serial.println(ultraDuration * 0.034 / 2);

    // Calculating the distance
    return ultraDuration * 0.034 / 2;
    
  }

  void extendArm(bool reverse) {
    digitalWrite(ARM_EXTEND_PIN, reverse ? HIGH : LOW);
    digitalWrite(ARM_EXTEND_PIN_2, reverse ? LOW : HIGH);
  }

  void stopArm() {
    digitalWrite(ARM_EXTEND_PIN, HIGH);
    digitalWrite(ARM_EXTEND_PIN_2, HIGH);
  }

  void startVibration() {
    digitalWrite(VIBRATION_MOTOR_PIN, HIGH);
  }

  void stopVibration() {
    digitalWrite(VIBRATION_MOTOR_PIN, LOW);
  }

  // void openClaw() {
  //   // clawMotor.write(CLAW_OPEN_VALUE);
  // }

  // void closeClaw() {
  //   // clawMotor.write(CLAW_CLOSED_VALUE);
  // }

  // checks if an object is detected by the ultrasonic sensor within distance cm range
  bool isObstacleDetected(float distance = ULTRA_DISTANCE_WARNING) {
    return GetUltraDistance() < distance;
  }

  void changeDirection() {
    Vector2 position = getPosition();

    float nextX = position.x;
    float nextY = position.y;
    Vector2 nextPosition;
    if(position.y > 0.0625 && isObstacleDetected()){
      nextY -= MOVE_LEFT_VALUE;
      isObstacleDetected();
      nextPosition = (Vector2){nextX, nextY};
      localMove(nextPosition);
      
    } else if(position.y < 1.9375 && isObstacleDetected()){
      nextY += MOVE_RIGHT_VALUE;
      isObstacleDetected();
      nextPosition = (Vector2){nextX, nextY};
      localMove(nextPosition);
      
    } else {
      nextX -= MOVE_FORWARD_VALUE;
      nextPosition = (Vector2){nextX, nextY};
      localMove(nextPosition);
      changeDirection();
    }
  }

  void moveForward(){
    Vector2 nextPosition = getPosition();
    float nextX = nextPosition.x;
    float nextY = nextPosition.y + MOVE_FORWARD_VALUE;
    nextPosition = (Vector2){nextX, nextY};
    if(!isObstacleDetected())
      localMove(nextPosition);

    while(!isObstacleDetected())
      delay(20);
    
    stop();
  }

  // moves the otv in a direction relative to the otv's current orientation (+y is forward, +x is right)
  void localMove(Vector2 direction) { // call stop() to stop this guy
  
    direction = direction.normalized();
    speed = speed > 1 ? 1 : (speed < 0 ? 0 : speed);

    float m1Speed = direction.x * speed;
    float m2Speed = (-direction.x * 0.5f + 0.866f * direction.y) * speed; 
    float m3Speed = (-direction.x * 0.5f - 0.866f * direction.y) * speed;

    digitalWrite(MOTOR_1_DIR_PIN, m1Speed > 0 ? LOW : HIGH); // assuming HIGH means reverse
    digitalWrite(MOTOR_2_DIR_PIN, m2Speed > 0 ? LOW : HIGH);
    digitalWrite(MOTOR_3_DIR_PIN, m3Speed > 0 ? LOW : HIGH);

    digitalWrite(MOTOR_1_DIR_PIN_2, m1Speed > 0 ? HIGH : LOW); // assuming HIGH means reverse
    digitalWrite(MOTOR_2_DIR_PIN_2, m2Speed > 0 ? HIGH : LOW);
    digitalWrite(MOTOR_3_DIR_PIN_2, m3Speed > 0 ? HIGH : LOW);

    analogWrite(MOTOR_1_PWM_PIN, fabsf(m1Speed) * 255); // assuming direction components and thus speed is between -1 and 1
    analogWrite(MOTOR_2_PWM_PIN, fabsf(m2Speed) * 255);
    analogWrite(MOTOR_3_PWM_PIN, fabsf(m3Speed) * 255);
  
  }

  // moves the otv in a given direction according to the vision system's coordinate system
  void globalMove(Vector2 direction) {
    // transform direction to otv local space
    direction = direction.rotatedBy(-getRotation() + M_PI_2); // X IS THE LONG AXIS
    localMove(direction);
  }

  // stops all propulsion motors
  void stop() {
    analogWrite(MOTOR_1_PWM_PIN, 0);
    analogWrite(MOTOR_2_PWM_PIN, 0);
    analogWrite(MOTOR_3_PWM_PIN, 0);
  }

  // rotates the otv clockwise or counterclockwise
  void rotate(bool reverse) { // again, call stop here to stop it

    speed = speed > 1 ? 1 : (speed < 0 ? 0 : speed);

    digitalWrite(MOTOR_1_DIR_PIN, reverse ? HIGH : LOW);
    digitalWrite(MOTOR_2_DIR_PIN, reverse ? HIGH : LOW);
    digitalWrite(MOTOR_3_DIR_PIN, reverse ? HIGH : LOW);

    digitalWrite(MOTOR_1_DIR_PIN_2, reverse ? LOW : HIGH);
    digitalWrite(MOTOR_2_DIR_PIN_2, reverse ? LOW : HIGH);
    digitalWrite(MOTOR_3_DIR_PIN_2, reverse ? LOW : HIGH);

    analogWrite(MOTOR_1_PWM_PIN, speed * 255);
    analogWrite(MOTOR_2_PWM_PIN, speed * 255);
    analogWrite(MOTOR_3_PWM_PIN, speed * 255);

  }

  // dictates how often the otv will recalculate offset from desired rotation/position
  #define CLOSE_ENOUGH_POLL_MS 20

  // moves the otv to a position according to the vision system
  void moveTo(Vector2 targetPosition) {
    // globalMove(targetPosition - getPosition());

    while (!closeEnough(targetPosition)) {
      globalMove(targetPosition - getPosition());
      delay(CLOSE_ENOUGH_POLL_MS);
      stop();
      delay(CLOSE_ENOUGH_POLL_MS/2);
    }

    stop();
  }

  // moves the otv to a position or towards that position until an obstacle is detected
  void moveToUntilObstacle(Vector2 targetPosition) {
    // globalMove(targetPosition - getPosition());

    while (!closeEnough(targetPosition) && !isObstacleDetected(3)) {
      globalMove(targetPosition - getPosition());
      delay(CLOSE_ENOUGH_POLL_MS);
      stop();
      delay(CLOSE_ENOUGH_POLL_MS/2);
    }

    stop();
  }

  /*
  void moveBy(Vector2 position){
    globalMove(position - getPosition());

    while(!isObstacleDetected())
      delay(CLOSE_ENOUGH_POLL_MS);
    
    stop();
  }
  */

  // rotates the otv until it reaches the desired angle
  void rotateTo(float angle) {
    // rotate(fabsf(normalizeAngle(getRotation() - angle)) > (float) M_PI);

    while (!closeEnough(angle))
    {
      float dif = fabsf(normalizeAngle(getRotation() - angle, 2*M_PI));
      
      rotate(min(2*M_PI - dif, dif) < M_PI);
      delay(CLOSE_ENOUGH_POLL_MS);
      stop();
      delay(CLOSE_ENOUGH_POLL_MS);
    }

    stop();
  }

  // the distance/angle for which the otv will stop trying to get closer to the desired position/orientation
  #define CLOSE_ENOUGH_DIST 0.05f // 5 cm
  #define CLOSE_ENOUGH_ANGLE (float) M_PI / 50; 

  // return if the otv is within defined thresholds of a given position/orientation
  private:
  bool closeEnough(Vector2 position) {
    return (position - getPosition()).magnitude() <= CLOSE_ENOUGH_DIST;
  }

  bool closeEnough(float angle) {
    float dif = fabsf(normalizeAngle(getRotation() - angle, 2*M_PI));
    return min(M_PI*2 - dif, dif) <= CLOSE_ENOUGH_ANGLE;
  }

};

#endif