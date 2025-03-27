#include <math.h>
#include "drivetrain.h"
#include "util.h"
#include "Enes100.h"

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
      navigationApproach();
    break;

    case NAV_OBSTACLES:
    // navigate past the obstacles with ultrasonic sensor
      navigateObstacles();
    break;

    case NAV_ENTER_ENDZONE:
    // go under the limbo
      navigateToEndzone();
    break;
  }

}

// precondition: facing obstacles (should be done in nav approach)
// Testing based constant parameters
#define NUM_OBSTACLE_POSITIONS 6
#define OBSTACLE_X_COORD_CLEARANCE 0.3

const Vector2 obstacleScanPositions[NUM_OBSTACLE_POSITIONS] =
  {
    {1.1f, 1.1f}, {1.1f, 1}, {1.1f, 0.5f},
    {1.9f, 1.5f}, {1.9f, 1}, {1.9f, 0.5f}
  }; // set up 0.4m away from center of each obstacle on the x axis

void navigateObstacles() {

  for (int i = 0; i < NUM_OBSTACLE_POSITIONS / 2; i++) {
    otv.moveTo(obstacleScanPositions[i]);

    if (!otv.isObstacleDetected()) {
      // move forward
      otv.moveTo(otv.getPosition() + (Vector2){ OBSTACLE_X_COORD_CLEARANCE, 0 });
      break;
    }
  }

  for(int i = 2; i < NUM_OBSTACLE_POSITIONS; i++){
    otv.moveTo(obstacleScanPositions[i]);

    if(!otv.isObstacleDetected()) {
      Vector2 position = otv.getPosition();
      float nextY = position.y + OBSTACLE_X_COORD_CLEARANCE;
      Vector2 nextPosition = (Vector2){position.x, nextY};
      otv.moveTo(nextPosition);
      break;
    }
  }

  state = NAV_ENTER_ENDZONE;

}


// asuming not going to hit anything on the way
const Vector2 obstacleApproachPosition = { 1, 1 };

void navigationApproach() {
  otv.moveTo(obstacleApproachPosition);// idk if moving to a position is the best way of doing this
  otv.rotateTo(0); // face the obstacles

  state = NAV_OBSTACLES;
}

const Vector2 endzoneEntryPosition = { 2.8f, 0.5f };
const Vector2 endzonePosition = { 3.7f, 0.5f };

void navigateToEndzone() {
  otv.moveTo(endzoneEntryPosition);
  otv.moveTo(endzonePosition);
}
