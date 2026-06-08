#include "common/clock.h"
#include "common/msg.h"
#include "common/udp_socket.h"

#include <chrono>
#include <iostream>
#include <thread>

static constexpr uint16_t LEFT_PORT  = 9001;
static constexpr uint16_t RIGHT_PORT = 9002;

int main() {
    sim_clock  clock(0.0f, 0);
    udp_socket sock(RIGHT_PORT);

    std::puts("hello right");

    std::thread rx([&] {
        ts_msg msg{};
        while (true) {
            if (sock.recv(&msg, sizeof(msg), 2000) == sizeof(msg))
                std::cout << "[right] rx from left:  " << msg.ts_us << " us\n";
        }
    });

    while (true) {
        ts_msg msg{};
        msg.node_id = 1;
        msg.ts_us   = clock.now_us();
        sock.send_to("127.0.0.1", LEFT_PORT, &msg, sizeof(msg));
        std::cout << "[right] tx: " << msg.ts_us << " us\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    rx.join();
}
