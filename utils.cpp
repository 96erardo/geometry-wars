#include<cmath>
#include<cstdlib>
#include "utils.h"

int between (int max, int min) {
  return (rand() % (1 + max - min)) + min;
}

float between (float max, float min) {
  return std::fmod(rand(), (1 +  max - min)) + min;
}