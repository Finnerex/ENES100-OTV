#include <math.h>
#include "drivetrain.h"
#include "util.h"

// Testing based constant parameters
#define NUM_OBSTACLE_POSITIONS 6
const Vector2 obstacleScanPositions[NUM_OBSTACLE_POSITIONS] = { {0, 0}, {0, 0} /*...*/ }; 

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
}

void loop() {
  otv.updateTransform();

  switch (state) {
    case MIS_APPROACH:
    break;

    case MIS_SEARCH_PLANTABLE:
    break;

    case MIS_FOUND_PLANTABLE:
    break;

    case MIS_PICKUP_SAMPLE:
    break;

    case NAV_APPROACH:
    break;

    case NAV_OBSTACLES:
      navigateObstacles();
    break;

    case NAV_ENTER_ENDZONE:
    break;
  }

}

// precondition: facing obstacles (should be done in nav approach)
void navigateObstacles() {

  bool onSecondRow = false;

  for (int i = 0; i < NUM_OBSTACLE_POSITIONS; i++) {
    otv.moveTo(obstacleScanPositions[i]);
    if (true/* see obstacle */) continue;

    // move forward

    if (onSecondRow) return;
    onSecondRow = true;

  }

  state = NAV_ENTER_ENDZONE;

}
