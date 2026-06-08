#include "common/clock.h"
#include <cstdio>
#include <iostream>
#include <thread>

int main() {
    sim_clock clock(20.0f, 5000);
    puts("hello right");
    while (true) {
        std::cout << clock.now_us() << '\n';
        std::this_thread::sleep_for(
        std::chrono::seconds(1));
    }
}
