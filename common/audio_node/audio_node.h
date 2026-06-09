#pragma once
#include <atomic>
#include <thread>
#include "common/comms/udp_comms.h"
#include "common/clock/clock.h"

class audio_node {
public:
    audio_node(uint16_t own_port, uint16_t peer_port);
    ~audio_node();

    // left calls this: sends START to peer, then begins rx loop
    void start();

    // right calls this: blocks until START is received, then begins rx loop
    void wait_for_start();

    bool is_running() const;

private:
    void run_rx_loop();

    udp_comms        comms_;
    sim_clock        clock_;
    std::thread      rx_thread_;
    std::atomic_bool running_{false};
};
