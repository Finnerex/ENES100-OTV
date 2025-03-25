#include <Math.h>
#include "Arduino.h"
#ifndef DRIVETRAIN_H
#define DRIVETRAIN_H

#include <math.h>
#include "util.h"

// Motor pin defines
#define MOTOR_1_PWM_PIN 3
#define MOTOR_2_PWM_PIN 5
#define MOTOR_3_PWM_PIN 6

#define MOTOR_1_DIR_PIN 0
#define MOTOR_2_DIR_PIN 1
#define MOTOR_3_DIR_PIN 2

class Drivetrain {
  
  Vector2 position;
  float rotation;

  public:
    float speed;

  public:
    Drivetrain() {}

  void updateTransform() {
    // set position & rotation from wifi module / vision system
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
    direction = direction.rotatedBy(-rotation);
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

    analogWrite(MOTOR_2_PWM_PIN, speed * 255);
    analogWrite(MOTOR_2_PWM_PIN, speed * 255);
    analogWrite(MOTOR_3_PWM_PIN, speed * 255);

  }

  #define CLOSE_ENOUGH_POLL_MS 20

  void moveTo(Vector2 position) {
    globalMove(position - this->position);

    while (!closeEnough(position))
      delay(CLOSE_ENOUGH_POLL_MS);
    
    stop();

  }

  void rotateTo(float angle) {
    rotate(normalizeAngle(rotation - angle) < (float) M_PI);

    while (!closeEnough(angle))
      delay(CLOSE_ENOUGH_POLL_MS);

    stop();
  }

  #define CLOSE_ENOUGH_SQR_DIST 4
  #define CLOSE_ENOUGH_ANGLE (float) M_PI / 32;

  private:
  bool closeEnough(Vector2 position) {
    return (position - this->position).sqrMagnitude() <= CLOSE_ENOUGH_SQR_DIST;
  }

  bool closeEnough(float angle) {
    return normalizeAngle(rotation - angle) <= CLOSE_ENOUGH_ANGLE;
  }

};

#endif