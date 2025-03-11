#include <math.h>


#define MOTOR_1_PWM_PIN 9
#define MOTOR_2_PWM_PIN 10
#define MOTOR_3_PWM_PIN 11

#define MOTOR_1_DIR_PIN 7
#define MOTOR_2_DIR_PIN 8
#define MOTOR_3_DIR_PIN 12

struct Vector2 {
  float x;
  float y;
};

class Drivetrain {
  
  Vector2 position;
  float rotation;

  public:
    Drivetrain() {}

  void updateTransform() {
    // set position & rotation from wifi module / vision system
  }

  void localMove(Vector2 direction/*, time or distance (or distance taken from first param*/) {
    // i would have to check if this is a global or local based direction
    float m1Speed = direction.x;
    float m2Speed = -direction.x * 0.5f + 0.866f * direction.y; 
    float m3Speed = -direction.x * 0.5f - 0.866f * direction.y;

    digitalWrite(MOTOR_1_DIR_PIN, m1Speed > 0 ? LOW : HIGH); // assuming HIGH means reverse
    digitalWrite(MOTOR_2_DIR_PIN, m2Speed > 0 ? LOW : HIGH);
    digitalWrite(MOTOR_3_DIR_PIN, m3Speed > 0 ? LOW : HIGH);

    analogWrite(MOTOR_1_PWM_PIN, fabsf(m1Speed) * 255); // assuming direction components and thus speed is between -1 and 1
    analogWrite(MOTOR_2_PWM_PIN, fabsf(m2Speed) * 255);
    analogWrite(MOTOR_3_PWM_PIN, fabsf(m3Speed) * 255);

  } // probably have overloads for angle instead of direction and for like localMove also move to position


  void localMoveUntil(Vector2 direction, bool* predicate) {
    while (!predicate()) {
      localMove(direction);
    }
  }

};

Drivetrain otv;


void setup() {
  // put your setup code here, to run once:
  
}

void loop() {
  // put your main code here, to run repeatedly:
  otv.updateTransform();

}
