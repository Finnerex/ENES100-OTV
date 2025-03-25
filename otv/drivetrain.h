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

class Drivetrain {
  
  Vector2 position;
  float rotation;
  bool obstacleDetected;
  //Ultrasonic Range Finder pin defines + variables
  const int trigPin = 10;
  const int echoPin = 9;
  long ultraDuration;
  int ultraDistance;
  #define ULTRA_DISTANCE_WARNING 2

  #define MOVE_FORWARD_VALUE 0.0010
  #define MOVE_LEFT_VALUE 0.0010
  #define MOVE_RIGHT_VALUE 0.0010

  public:
    float speed;

  public:
    Drivetrain() {}

  void updateTransform() {
    // set position & rotation from wifi module / vision system
    float x, y, t; bool v; // Declare variables to hold the data
    //Enes100.getX will make sure you get the latest data available to you about your OTV's location.
    //The first time getX is called, X, Y, theta and visibility are queried and cached.
    //Subsequent calls return from the cache, so there is no performance gain to saving the function response to a variable.

    x = Enes100.getX();  // Your X coordinate! 0-4, in meters, -1 if no aruco is not visibility (but you should use Enes100.isVisible to check that instead)
    y = Enes100.getY();  // Your Y coordinate! 0-2, in meters, also -1 if your aruco is not visible.
    t = Enes100.getTheta();  //Your theta! -pi to +pi, in radians, -1 if your aruco is not visible.
    v = Enes100.isVisible(); // Is your aruco visible? True or False.

    if (v) { // If the ArUco marker is visible
        Enes100.print(x); // print out the location
        Enes100.print(",");
        Enes100.print(y);
        Enes100.print(",");
        Enes100.println(t);
    }
    else // otherwise
        Enes100.println("Not visible"); // print not visible
    
     Enes100.mission(LOCATION, 'A'); //This is how you should send the location of the mission site that contains a plantable substrate.

    updateDistance();

    if(obstacleDetected)
      changeDirection();
    else {
      Vector2 forwardDirection = {x + 0.0010, y};
      localMove(forwardDirection);
    }

    delay(1000);
  }

  void updateDistance(){
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    ultraDuration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    ultraDistance = ultraDuration * 0.034 / 2;
    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(ultraDistance);
  }

  void isObstacleDetected(){

    if(ultraDistance > ULTRA_DISTANCE_WARNING)
      obstacleDetected = true;
    else
      obstacleDetected = false;
    
  }

  void changeDirection(){
    float nextX = x;
    float nextY = y;
    Vector2 nextPosition;
    if(y > 0.0625 && obstacleDetected){
      nextY -= MOVE_LEFT_VALUE;
      isObstacleDetected();
      nextPosition = (Vector2){nextX, nextY};
      localMove(nextPosition);

    } else if (y < 1.9375 && obstacleDetected) {
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