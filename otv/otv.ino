#include <math.h>
#include "drivetrain.h"
#include "util.h"
#include "Enes100.h"

// Testing based constant parameters
#define NUM_OBSTACLE_POSITIONS 6
const Vector2 obstacleScanPositions[NUM_OBSTACLE_POSITIONS] = { {0, 0}, {0, 0} /*...*/ }; 

#define OBSTACLE_X_COORD_CLEARANCE 0.5

// different states for control
// can be reduced or expanded idk
enum State {
  MIS_APPROACH,
  MIS_SEARCH_PLANTABLE,
  MIS_FOUND_PLANTABLE,
  MIS_PICKUP_SAMPLE,
  NAV_APPROACH,
  NAV_OBSTACLES,
  NAV_ENTER_ENDZONE
};


// Global Variables
Drivetrain otv;
State state = MIS_APPROACH;

void setup() {
  otv.speed = 0.2f; // for testing, lower total speed

  // Initialize Aruco Wifi
  // Initialize Enes100 Library
  // Team Name, Mission Type, Marker ID, Room Number, Wifi Module TX Pin, Wifi Module RX Pin
  Enes100.begin("Teem Slyde", SEED, 205, 1116, A0, A1);
  // At this point we know we are connected.
  Enes100.println("Connected...");
}

bool sent = false;

void loop() {

  switch (state) {
    case MIS_APPROACH:
    // locate where otv is, rotate and move to mission zone
    break;

    case MIS_SEARCH_PLANTABLE:
    // search each plot for plantable substrate (orzo) using color sensor
    break;

    case MIS_FOUND_PLANTABLE:
    // plant the seed in the plot with plantable substrate
    break;

    case MIS_PICKUP_SAMPLE:
    // move to a different plot and get 10g of gravel
    break;

    case NAV_APPROACH:
    // move away from mission zone, face obstacles
    break;

    case NAV_OBSTACLES:
    // navigate past the obstacles with ultrasonic sensor
      navigateObstacles();
    break;

    case NAV_ENTER_ENDZONE:
    // go under the limbo
    break;
  }

}

// precondition: facing obstacles (should be done in nav approach)
void navigateObstacles() {

  bool onSecondRow = false;

  for (int i = 0; i < 3; i++) {
    otv.moveTo(obstacleScanPositions[i]);
    if (!isObstacleDetected()){
      // move forward
      Vector2 position = otv.getPosition;
      float nextY = position.y + OBSTACLE_X_COORD_CLEARANCE;
      Vector2 nextPosition = (Vector2){position.x, nextY};
      moveTo(nextPosition);
      onSecondRow = true;
      break;
    }
  }

  for(int i = 2; i < NUM_OBSTACLE_POSITIONS; i++){
    otv.moveTo(obstacleScanPositions[i]);
    if(!isObstacleDetected()){
      Vector2 position = otv.getPosition;
      float nextY = position.y + OBSTACLE_X_COORD_CLEARANCE;
      Vector2 nextPosition = (Vector2){position.x, nextY};
      moveTo(nextPosition);
      break;
    }
  }

  state = NAV_ENTER_ENDZONE;

}

void navigateEndzone(){

}
