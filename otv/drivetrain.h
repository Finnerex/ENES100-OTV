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

  void localMove(Vector2 direction/*, time or distance (or distance taken from first param)*/) {
    // i would have to check if this is a global or local based direction
    float m1Speed = direction.x * speed;
    float m2Speed = (-direction.x * 0.5f + 0.866f * direction.y) * speed; 
    float m3Speed = (-direction.x * 0.5f - 0.866f * direction.y) * speed;

    digitalWrite(MOTOR_1_DIR_PIN, m1Speed > 0 ? LOW : HIGH); // assuming HIGH means reverse
    digitalWrite(MOTOR_2_DIR_PIN, m2Speed > 0 ? LOW : HIGH);
    digitalWrite(MOTOR_3_DIR_PIN, m3Speed > 0 ? LOW : HIGH);

    analogWrite(MOTOR_1_PWM_PIN, fabsf(m1Speed) * 255); // assuming direction components and thus speed is between -1 and 1
    analogWrite(MOTOR_2_PWM_PIN, fabsf(m2Speed) * 255);
    analogWrite(MOTOR_3_PWM_PIN, fabsf(m3Speed) * 255);

    // wait time / distance

    stop();

  } // probably have overloads for angle instead of direction and for like localMove also move to position

  void stop() {
    analogWrite(MOTOR_1_PWM_PIN, 0);
    analogWrite(MOTOR_2_PWM_PIN, 0);
    analogWrite(MOTOR_3_PWM_PIN, 0);
  }

  void localMoveUntil(Vector2 direction, bool* predicate(void)) { // probably completely unnecessary
    while (!predicate()) {
      localMove(direction);
    }
  }

  void moveTo(Vector2 position) {

  }

};

#endif