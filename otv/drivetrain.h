#ifndef DRIVETRAIN_H
#define DRIVETRAIN_H

#include <math.h>
#include "util.h"
#include "Enes100.h"

// Motor pin defines
#define MOTOR_1_PWM_PIN 3
#define MOTOR_2_PWM_PIN 5
#define MOTOR_3_PWM_PIN 6

#define MOTOR_1_DIR_PIN 0
#define MOTOR_2_DIR_PIN 1
#define MOTOR_3_DIR_PIN 2

#define ULTRA_TRIGGER_PIN 10
#define ULTRA_ECHO_PIN 9

class Drivetrain {
  
  //Ultrasonic Range Finder pin defines + variables
  long ultraDuration;

  #define ULTRA_DISTANCE_WARNING 2

  #define MOVE_FORWARD_VALUE 0.0010
  #define MOVE_LEFT_VALUE 0.0010
  #define MOVE_RIGHT_VALUE 0.0010

  public:
    float speed;

  public:
    Drivetrain() {}

  Vector2 getPosition() {
    while (!Enes100.isVisible())
      delay(20);

    return { Enes100.getX(), Enes100.getY() };
  }

  float getRotation() {
    while (!Enes100.isVisible())
      delay(20);

    return Enes100.getTheta();
  }

  float GetUltraDistance() {
    // Clears the trigPin
    digitalWrite(ULTRA_TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(ULTRA_TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRA_TRIGGER_PIN, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    ultraDuration = pulseIn(ULTRA_ECHO_PIN, HIGH);
    // Calculating the distance
    return ultraDuration * 0.034 / 2;
    // Prints the distance on the Serial Monitor
    // Serial.print("Distance: ");
    // Serial.println(ultraDistance);
  }

  bool isObstacleDetected() {
    return GetUltraDistance() > ULTRA_DISTANCE_WARNING;
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
      delay(CLOSE_ENOUGH_POLL_MS);
    
    stop();
  }

  void localMove(Vector2 direction) { // call stop() to stop this guy
  
    direction = direction.normalized();

    float m1Speed = direction.x * speed;
    float m2Speed = (-direction.x * 0.5f + 0.866f * direction.y) * speed; 
    float m3Speed = (-direction.x * 0.5f - 0.866f * direction.y) * speed;

    digitalWrite(MOTOR_1_DIR_PIN, m1Speed > 0 ? LOW : HIGH); // assuming HIGH means reverse
    digitalWrite(MOTOR_2_DIR_PIN, m2Speed > 0 ? LOW : HIGH);
    digitalWrite(MOTOR_3_DIR_PIN, m3Speed > 0 ? LOW : HIGH);

    analogWrite(MOTOR_1_PWM_PIN, fabsf(m1Speed) * 255); // assuming direction components and thus speed is between -1 and 1
    analogWrite(MOTOR_2_PWM_PIN, fabsf(m2Speed) * 255);
    analogWrite(MOTOR_3_PWM_PIN, fabsf(m3Speed) * 255);

  }

  void globalMove(Vector2 direction) {
    // transform direction to otv local space
    direction = direction.rotatedBy(-getRotation());
    localMove(direction);
  }

  void stop() {
    analogWrite(MOTOR_1_PWM_PIN, 0);
    analogWrite(MOTOR_2_PWM_PIN, 0);
    analogWrite(MOTOR_3_PWM_PIN, 0);
  }

  void rotate(bool reverse) { // again, call stop here to stop it

    digitalWrite(MOTOR_1_DIR_PIN, reverse ? HIGH : LOW);
    digitalWrite(MOTOR_2_DIR_PIN, reverse ? HIGH : LOW);
    digitalWrite(MOTOR_3_DIR_PIN, reverse ? HIGH : LOW);

    analogWrite(MOTOR_1_PWM_PIN, speed * 255);
    analogWrite(MOTOR_2_PWM_PIN, speed * 255);
    analogWrite(MOTOR_3_PWM_PIN, speed * 255);

  }

  #define CLOSE_ENOUGH_POLL_MS 20

  void moveTo(Vector2 position) {
    globalMove(position - getPosition());

    while (!closeEnough(position))
      delay(CLOSE_ENOUGH_POLL_MS);
    
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

  void rotateTo(float angle) {
    rotate(normalizeAngle(getRotation() - angle) < (float) M_PI);

    while (!closeEnough(angle))
      delay(CLOSE_ENOUGH_POLL_MS);

    stop();
  }

  #define CLOSE_ENOUGH_SQR_DIST 4
  #define CLOSE_ENOUGH_ANGLE (float) M_PI / 32;

  private:
  bool closeEnough(Vector2 position) {
    return (position - getPosition()).sqrMagnitude() <= CLOSE_ENOUGH_SQR_DIST;
  }

  bool closeEnough(float angle) {
    return normalizeAngle(getRotation() - angle) <= CLOSE_ENOUGH_ANGLE;
  }

};

#endif