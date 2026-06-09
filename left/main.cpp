/*
# build both
  bazel build //left:left //right:right

  bazel run //left:left
  bazel run //right:right
*/
#include "common/audio_node/audio_node.h"


static constexpr uint16_t LEFT_PORT  = 9001;
static constexpr uint16_t RIGHT_PORT = 9002;

int main() {
    audio_node left_node(LEFT_PORT, RIGHT_PORT);
    left_node.start();

    while (left_node.is_running()) {
        ;
    }
}



#if 0

    sim_clock    clock(0.0f, 0);
    udp_socket   sock(LEFT_PORT);

    std::puts("hello left");

    std::thread rx([&] {
        ts_msg msg{};
        while (true) {
            if (sock.recv(&msg, sizeof(msg), 2000) == sizeof(msg))
                std::cout << "[left]  rx from right: " << msg.ts_us << " us\n";
        }
    });

    while (true) {
        ts_msg msg{};
        msg.node_id = 0;
        msg.ts_us   = clock.now_us();
        sock.send_to("127.0.0.1", RIGHT_PORT, &msg, sizeof(msg));
        std::cout << "[left]  tx: " << msg.ts_us << " us\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    rx.join();
}
#endif