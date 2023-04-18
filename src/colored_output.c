
#include "colored_output.h"


void black () {
  printf("\033[1;30m");
}

void red () {
  printf("\033[1;31m");
}

void green () {
  printf("\033[1;32m");
}

void blue () {
  printf("\034[1;32m");
}

void reset () {
  printf("\033[0m");
}
