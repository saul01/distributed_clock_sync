#pragma once
#include <atomic>
#include <thread>
#include "common/comms/udp_comms.h"
#include "common/clock/clock.h"

class audio_node {
public:
    audio_node(uint16_t own_port, uint16_t peer_port,
               float ppm_error = 0.0f, int64_t offset_us = 0);
    ~audio_node();

    // left calls this: sends START, spawns rx + sync threads
    void start();

    // right calls this: blocks until START received, spawns rx thread
    void wait_for_start();

    bool is_running() const;

private:
    enum class role_t { UNKNOWN, LEFT, RIGHT };

    void run_rx_loop();
    void run_sync_loop();                          // left only: periodic SYNC_REQ sender
    void handle_sync_req(const sync_message& req); // right: record t2/t3, reply SYNC_RESP
    void handle_sync_resp(const sync_message& resp);// left: compute offset, send SYNC_ADJ
    void handle_sync_adj(const sync_message& adj);  // right: apply PI to own clock
    void apply_pi(double offset_us);

    udp_comms             comms_;
    sim_clock             clock_;
    std::thread           rx_thread_;
    std::thread           sync_thread_;
    std::atomic_bool      running_{false};
    std::atomic<uint64_t> seq_{0};
    role_t                role_{role_t::UNKNOWN};
    double                pi_integral_{0.0};
};
