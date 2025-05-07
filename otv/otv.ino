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

  // Serial.begin(9600);

  // clawMotor.write(CLOSE_CLAW));
  // clawMotor.attach(CLAW_PWM_PIN);

  if (colorSensor.init())
    Serial.println("Sensor Initialization Successful\n\r");
  else
    Serial.println("It does not work!!!!!!!!!!!!!!!!!!");


  
  // Initialize Aruco Wifi
  // Initialize Enes100 Library
  // Team Name, Mission Type, Marker ID, Room Number, Wifi Module TX Pin, Wifi Module RX Pin
  Enes100.begin("Teem Slyde", SEED, 11, 1116, A0, A1);
  Enes100.println("Connected...");

  // otv.extendArm(true);
  //Entire navigation code
  // approachMissionSite();
  // delay(1000);
  // identifySeedPlot();
  // char plot = identifySeedPlot();
  // Enes100.println(plot);
  // // navigationApproach();
  // delay(500);
  // navigateObstacles();
  // delay(500);
  // navigateToEndzone();

  
  // Serial.print("x: ");
  // Serial.print(otv.getPosition().x);
  // Serial.print(" y: ");
  // Serial.print(otv.getPosition().y);
  
  float redVal = detectColor();
  Enes100.println(redVal);

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

#define PLOT_INDEX 0
#define ORZO_TOLERANCE 35
#define MOVE_BETWEEN_PLOTS 0.083 //3.26772 inches; moves from the middle of one plot to the next
#define ADJUST_TO_PLOT 0.0415 //moves from middle of mission site to middle of first plot

bool startAtA; //determines the starting position of the robot

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

void identifySeedPlot(){ //for mission site B; starts at B
  char plotIndexes[] = {'A', 'B', 'C', 'D'};
  bool orzoFound = false;
  char plotChar;
  if(!startAtA){
    while(!orzoFound){
      float redValue = detectColor(); //detects color value of plot B
      if(redValue > ORZO_TOLERANCE){
        orzoFound = true;
        plotChar = 'B';
        Enes100.print("Plantable substrate found at plot: ");
        Enes100.println("B");
        break;
      }

      otv.moveTo({otv.getPosition().x, otv.getPosition().y + 0.1}); //move back 2 inches
      otv.moveTo({otv.getPosition().x - MOVE_BETWEEN_PLOTS, otv.getPosition().y-0.05}); //moves to plot C
      redValue = detectColor(); //detects color value of plot C
      if(redValue > ORZO_TOLERANCE){
        orzoFound = true;
        plotChar = 'C';
        Enes100.print("Plantable substrate found at plot: ");
        Enes100.println("C");
        break;
      }

      //moves to plot D on the other side of the mission site
      otv.moveTo({0.04, 0.81});
      otv.moveTo({0.04, 0.1});
      otv.moveTo({0.34 - MOVE_BETWEEN_PLOTS-0.02, 0.1}); //middle of other side - displacement
      otv.rotateTo(1); //rotate to face mission site
      otv.moveToUntilObstacle({0.34 - MOVE_BETWEEN_PLOTS, 0.5}); //get closer to mission site

      redValue = detectColor(); //detects the color value of plot D
      Enes100.println(redValue);
      if(redValue > ORZO_TOLERANCE){
        orzoFound = true;
        plotChar = 'D';
        Enes100.print("Plantable substrate found at plot: ");
        Enes100.println("D");
        break;
      }
      
      otv.moveTo({otv.getPosition().x, otv.getPosition().y - 0.0508});
      otv.moveTo({otv.getPosition().x + MOVE_BETWEEN_PLOTS, otv.getPosition().y}); //moves to plot A
      redValue = detectColor(); //detects color value of plot A
      Enes100.println(redValue);
      if(redValue > ORZO_TOLERANCE){
        orzoFound = true;
        plotChar = 'A';
        Enes100.print("Plantable substrate found at plot: ");
        Enes100.println("A");
        break;
      }

      //create: move back to plot A if not detected; will start the process again
    }
  } else{ //if mission site is at site A
      while(!orzoFound){
        float redValue = detectColor(); //detects color value of plot B
        if(redValue > ORZO_TOLERANCE){
          orzoFound = true;
          Enes100.print("Plantable substrate found at plot: ");
          Enes100.println("A");
          break;
        }

        otv.moveTo({otv.getPosition().x - MOVE_BETWEEN_PLOTS, otv.getPosition().y}); //moves to plot C
        redValue = detectColor(); //detects color value of plot C
        if(redValue > ORZO_TOLERANCE){
          orzoFound = true;
          Enes100.print("Plantable substrate found at plot: ");
          Enes100.println("D");
          break;
        }

        //moves to plot D on the other side of the mission site
        otv.moveTo({0.65, 1.24}); //moves to S1 //change coordinates
        otv.moveTo({0.65, 1.92}); //moves to S2 //change coordinates
        otv.moveTo({0.65 - MOVE_BETWEEN_PLOTS, 1.92}); //middle of other side - displacement //change coordinates
        otv.rotateTo(1); //rotate to face mission site
        otv.moveToUntilObstacle({0.65 - MOVE_BETWEEN_PLOTS, 0.5}); //get closer to mission site //change coordinates

        redValue = detectColor(); //detects the color value of plot D
        if(redValue > ORZO_TOLERANCE){
          orzoFound = true;
          Enes100.print("Plantable substrate found at plot: ");
          Enes100.println("C");
          break;
        }
        
        otv.moveTo({otv.getPosition().x + MOVE_BETWEEN_PLOTS, otv.getPosition().y}); //moves to plot A
        redValue = detectColor(); //detects color value of plot D
        if(redValue > ORZO_TOLERANCE){
          orzoFound = true;
          Enes100.print("Plantable substrate found at plot: ");
          Enes100.println("B");
          break;
        }

        //create: move back to plot A if not detected; will start the process again
      }
    }
  
  
  
  delay(500);

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

    // red /= brightness;
    // green /= brightness;
    // blue /=brightness;

    Serial.print("r: "); Serial.print(red);
    Serial.print(", g: "); Serial.print(green);
    Serial.print(", b: "); Serial.println(blue);

    delay(200);
  }
  return red;
}

void DANCE() {
  otv.rotate(true);
  delay(500);
  otv.rotate(false);
  delay(500);
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
