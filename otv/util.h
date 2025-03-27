#ifndef UTIL_H
#define UTIL_H

#include <Math.h>

// could use an actual vector library
struct Vector2 {
  
  float x;
  float y;

  Vector2 normalized() {
    float magnitude = this->magnitude();
    return { x / magnitude, y / magnitude };
  }

  float magnitude() {
    return sqrtf(x * x + y * y);
  }

  float sqrMagnitude() {
    return x * x + y * y;
  }

  Vector2 rotatedBy(float angle) {
    return
    {
      x * cosf(angle) - y * sinf(angle),
      x * sinf(angle) + y * cosf(angle)
    };
  }

  Vector2 operator - (Vector2 other) {
    return { x - other.x, y - other.y };
  }

  Vector2 operator + (Vector2 other) {
    return { x + other.x, y + other.y };
  }
};

float normalizeAngle(float angle) {
  while (angle > (float) M_PI) 
    angle -= (float) M_2_PI;

  while (angle < (float) -M_PI)
    angle += (float) M_2_PI;

  return angle;
}

#endif