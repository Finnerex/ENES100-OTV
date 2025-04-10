#include <math.h>
#include <Wire.h>
#include "drivetrain.h"
#include "util.h"
#include "Enes100.h"
#include "SparkFunISL29125.h"

// Global Variables
Drivetrain otv;
SFE_ISL29125 colorSensor;

void setup() {
  otv.speed = 0.9f; // for testing, lower total speed

  Serial.begin(9600);

  if (colorSensor.init())
    Serial.println("Sensor Initialization Successful\n\r");
  else
    Serial.println("It does not work!!!!!!!!!!!!!!!!!!");


  
  // Initialize Aruco Wifi
  // Initialize Enes100 Library
  // Team Name, Mission Type, Marker ID, Room Number, Wifi Module TX Pin, Wifi Module RX Pin
  // Enes100.begin("Teem Slyde", SEED, 11, 1120, A0, A1);
  // Enes100.println("Connected...");

  // navigationApproach();
  // delay(1000);
  // navigateObstacles();
  // delay(1000);
  // navigateToEndzone();

  
}

unsigned int rLow = 600, rHigh = 3600;
unsigned int gLow = 2300, gHigh = 5700;
unsigned int bLow = 2100, bHigh = 5200;


void loop() {

  float red = colorSensor.readRed();
  float green = colorSensor.readGreen();
  float blue = colorSensor.readBlue();

  red = map(red, rLow, rHigh, 0, 255);
  green = map(green, gLow, gHigh, 0, 255);
  blue = map(blue, bLow, bHigh, 0, 255);

  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);


  // float brightness = (0.299*red + 0.587*green + 0.114*blue);

  // red /= brightness;
  // green /= brightness;
  // blue /= brightness;
  

  Serial.print("r: "); Serial.print(red);
  Serial.print(", g: "); Serial.print(green);
  Serial.print(", b: "); Serial.println(blue);

  // float rp = red/255;
  // float gp = green/255;
  // float bp = blue/255;

  // float Cmax = max(rp, max(gp, bp));
  // float Cmin = min(rp, min(gp, bp));
  // float delta = Cmax - Cmin;
  
  // float hue;
  // if (Cmax == rp)
  //   hue = 60 * ((gp - bp) / delta);
  // else if (Cmax == gp)
  //   hue = 60 * (((bp - rp)/delta) + 2);
  // else if (Cmax == bp)
  //   hue = 60 * (((rp - gp)/delta) + 4);
  // else
  //   hue = -123098;

  // float value = Cmax;

  // Serial.print("hue: "); Serial.print(hue);
  // Serial.print(", value: "); Serial.println(value);

  delay(200);

  // orzo is around 1.05, 1.07, 0.5

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

const Vector2 endzoneEntryPosition = { 2.8f, 1.5f };
// const Vector2 endzonePosition = { 3.7f, 1.5f };

void navigateToEndzone() {
  otv.moveTo(endzoneEntryPosition);
  // otv.moveTo(endzonePosition);
  // loses vison system tracking under limbo, so move without using it here
  otv.rotateTo(0);
  otv.localMove({1, 0});
  delay(2000); // value probably needs tweaking
  otv.stop();
}

void DANCE() {
  otv.rotate(true);
  delay(500);
  otv.rotate(false);
  delay(500);
}
