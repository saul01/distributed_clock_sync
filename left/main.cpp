  /*
# build both
  bazel build //left:left //right:right

  bazel run //left:left
  bazel run //right:right
*/
#include "common/clock.h"
#include <cstdio>
#include <iostream>
#include <thread>

int main() {
    sim_clock clock(0.0f, 0);
    puts("hello left");

    while (true) {
        std::cout << clock.now_us() << '\n';
        std::this_thread::sleep_for(
        std::chrono::seconds(1));
    }

    return 0;
}
