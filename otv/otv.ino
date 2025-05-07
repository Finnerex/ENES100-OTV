#include <math.h>
#include <Wire.h>
#include "drivetrain.h"
#include "util.h"
#include "Enes100.h"
#include "SparkFunISL29125.h"
#include <Servo.h>  



// Global Variables
Otv otv;
SFE_ISL29125 colorSensor;

Servo clawMotor;

#define CLOSE_CLAW 50
#define OPEN_CLAW 10

void setup() {
  otv.speed = 0.9f; // for testing, lower total speed

  // clawMotor.write(CLOSE_CLAW));
  // clawMotor.attach(CLAW_PWM_PIN);

  // if (colorSensor.init())
  //   Serial.println("Sensor Initialization Successful\n\r");
  // else
  //   Serial.println("It does not work!!!!!!!!!!!!!!!!!!");


  // Initialize Aruco Wifi
  // Initialize Enes100 Library
  // Team Name, Mission Type, Marker ID, Room Number, Wifi Module TX Pin, Wifi Module RX Pin
  Enes100.begin("Teem Slyde", SEED, 11, 1116, A0, A1);
  Enes100.println("Connected...");

  // mission
  // approachMissionSite();
  // delay(500);

  // navigation
  // navigationApproach();
  // delay(500);
  // navigateObstacles();
  // delay(500);
  // navigateToEndzone();

  
// clawMotor.write(0);

  approachMissionSite();

  // otv.extendArm(false);
  // delay(3000);
  // clawMotor.write(CLOSE_CLAW);
  // delay(5000);
  // embedSeed();

  
}

unsigned int rLow = 600, rHigh = 3600;
unsigned int gLow = 2300, gHigh = 5700;
unsigned int bLow = 2100, bHigh = 5200;

void loop() {

  // otv.extendArm(false);
  // delay(3500);
  // otv.extendArm(true);
  // delay(3500);

  // DANCE();

}

// precondition: facing obstacles (should be done in nav approach)
// Testing based constant parameters
#define NUM_OBSTACLE_POSITIONS 6
#define OBSTACLE_X_COORD_CLEARANCE 0.6

const Vector2 obstacleScanPositions[NUM_OBSTACLE_POSITIONS] =
  {
    {0.94f, 1.5f}, {0.94f, 1}, {0.94f, 0.5f},
    {1.85f, 1.5f}, {1.85f, 1}, {1.85f, 0.5f}
  }; // set up 0.4m away from center of each obstacle on the x axis

void navigateObstacles() {

  for (int i = 0; i < NUM_OBSTACLE_POSITIONS / 2; i++) {
    otv.moveTo(obstacleScanPositions[i]);
    otv.rotateTo(0);
    delay(1000);

    if (!otv.isObstacleDetected()) {
      // move forward
      Enes100.println("Obstacle not detected, moving forward");
      otv.moveTo(otv.getPosition() + (Vector2){ OBSTACLE_X_COORD_CLEARANCE, 0 });
      otv.rotateTo(0);
      break;
    }
    Enes100.println("Obstacle detected, moving to next position");

  }

  for(int i = 3; i < NUM_OBSTACLE_POSITIONS; i++){
    otv.moveTo(obstacleScanPositions[i]);
    otv.rotateTo(0);

    if(!otv.isObstacleDetected()) {
      otv.moveTo(otv.getPosition() + (Vector2){ OBSTACLE_X_COORD_CLEARANCE, 0 });
      otv.rotateTo(0);
      break;
    }
  }


}


// asuming not going to hit anything on the way
const Vector2 obstacleApproachPosition = { 0.9f, 0.98f };

void navigationApproach() {
  otv.moveTo(obstacleApproachPosition);// idk if moving to a position is the best way of doing this
  otv.rotateTo(0); // face the obstacles

}

const Vector2 endzoneEntryPosition = { 3.2f, 1.5f };
// const Vector2 endzonePosition = { 3.7f, 1.5f };

void navigateToEndzone() {
  otv.moveTo(endzoneEntryPosition);
  otv.rotateTo(0);
  otv.moveTo(endzoneEntryPosition);


  // otv.moveTo(endzonePosition);
  // loses vison system tracking under limbo, so move without using it here
  otv.localMove({0, 1});
  
  delay(3200); // value probably needs tweaking
  otv.stop();
}

void approachMissionSite(){
  bool startAtA = false;
  if(otv.getPosition().y > 1) startAtA = true; //starting at position A if y coord > 1
  otv.rotateTo((startAtA ? -1 : 1)*M_PI/2);
    otv.moveToUntilObstacle({otv.getPosition().x, (startAtA ? 0.25 : 1.75)});
    // while(!otv.isObstacleDetected()){
    //   delay(1000);
    // }
    otv.stop();
}

void DANCE() {
  otv.rotate(true);
  delay(500);
  otv.rotate(false);
  delay(500);
}

#define PLOT_INDEX 0
#define ORZO_TOLERANCE 90

void identifySeedPlot(){
  char plotIndexes[] = {'A', 'B', 'C', 'D'};
  bool orzoFound = false;
  char plotChar = 'A';
  while(!orzoFound){
    float redValue = detectColor();
    if(redValue > ORZO_TOLERANCE){
      orzoFound = true;
      break;
    }else{
      
    }
  }

}

int detectColor(){
  float red;
  float green;
  float blue;
  for(int i=0; i < 8; i++){
    red = colorSensor.readRed();
    green = colorSensor.readGreen();
    blue = colorSensor.readBlue();

    red = map(red, rLow, rHigh, 0, 255);
    green = map(green, gLow, gHigh, 0, 255);
    blue = map(blue, bLow, bHigh, 0, 255);

    red = constrain(red, 0, 255);
    green = constrain(green, 0, 255);
    blue = constrain(blue, 0, 255);

    float brightness = (0.299*red + 0.587*green + 0.114*blue);

    Serial.print("r: "); Serial.print(red);
    Serial.print(", g: "); Serial.print(green);
    Serial.print(", b: "); Serial.println(blue);

    delay(200);
  }
  return red;
}

// should be over orzo before this is called
// assuming reverse: true -> down
void embedSeed() {
  otv.extendArm(true);
  delay(3500);
  otv.stopArm();
  // otv.openClaw();
  clawMotor.write(OPEN_CLAW);
  delay(1000);
  otv.extendArm(false);
  delay(2000);
  // otv.closeClaw();
  // clawMotor.write(CLOSE_CLAW);
  delay(1000);
  otv.stopArm();
}

// should be over rocks
void collectSample() {
  // otv.openClaw();
  clawMotor.write(OPEN_CLAW);
  delay(500);
  otv.extendArm(true);
  delay(3500);
  // otv.closeClaw();
  clawMotor.write(CLOSE_CLAW);
  delay(1000);
  otv.extendArm(false);
  delay(3500);
}
