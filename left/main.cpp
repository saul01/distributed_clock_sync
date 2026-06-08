/*
# build both
  bazel build //left:left //right:right

  bazel run //left:left
  bazel run //right:right
*/
#include "common/clock.h"
#include <cstdio>

int main() {
    puts("hello left");
    return 0;
}
