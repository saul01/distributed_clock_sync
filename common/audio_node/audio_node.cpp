#include "audio_node.h"
#include <iostream>
#include <thread>
#include <chrono>

// PI-style gains for right-side clock steering.
// Kp: fraction of measured offset applied directly to offset_us each round (phase correction).
// Kf: ppm change per microsecond of measured phase offset (frequency correction).
// Both applied to the current measurement — no windup risk.
static constexpr double Kp = 0.5;    // phase
static constexpr double Kf = 0.002;  // frequency
static constexpr double dt = 0.2;    // sync period in seconds — must match sleep in run_sync_loop

audio_node::audio_node(uint16_t own_port, uint16_t peer_port,
                       float ppm_error, int64_t offset_us)
    : comms_(own_port, peer_port), clock_(ppm_error, offset_us)
{
}

audio_node::~audio_node()
{
    running_ = false;
    if (rx_thread_.joinable())   rx_thread_.join();
    if (sync_thread_.joinable()) sync_thread_.join();
}

void audio_node::start()
{
    role_ = role_t::LEFT;

    sync_message msg{};
    msg.type = msg_type_t::START;
    comms_.send(msg);
    std::cout << "[left]  sent START" << std::endl;

    running_ = true;
    rx_thread_   = std::thread(&audio_node::run_rx_loop,  this);
    sync_thread_ = std::thread(&audio_node::run_sync_loop, this);
}

void audio_node::wait_for_start()
{
    role_ = role_t::RIGHT;

    while (true) {
        auto msg = comms_.receive(500);
        if (msg && msg->type == msg_type_t::START) {
            std::cout << "[right] received START" << std::endl;
            break;
        }
    }

    running_ = true;
    rx_thread_ = std::thread(&audio_node::run_rx_loop, this);
}

bool audio_node::is_running() const { return running_; }

// --- left: periodic SYNC_REQ sender ---

void audio_node::run_sync_loop()
{
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(dt * 1000)));
        if (!running_) break;

        sync_message req{};
        req.type  = msg_type_t::SYNC_REQ;
        req.seq   = seq_++;
        req.t1_us = clock_.now_us();
        comms_.send(req);
    }
}

// --- shared rx dispatch ---

void audio_node::run_rx_loop()
{
    const char* tag = (role_ == role_t::LEFT) ? "left" : "right";
    auto last_hb    = std::chrono::steady_clock::now();

    while (running_) {
        auto msg = comms_.receive(500);

        auto now = std::chrono::steady_clock::now();
        if (now - last_hb >= std::chrono::seconds(1)) {
            last_hb = now;
            if (role_ == role_t::LEFT) {
                std::cout << "[left]  sync error = " << last_offset_us_ << " us" << std::endl;
            } else {
                std::cout << "[right] sync error = " << last_offset_us_ << " us"
                          << "  ppm = " << clock_.ppm_error << std::endl;
            }
        }

        if (!msg) continue;

        switch (msg->type) {
            case msg_type_t::SYNC_REQ:  handle_sync_req(*msg);  break;
            case msg_type_t::SYNC_RESP: handle_sync_resp(*msg); break;
            case msg_type_t::SYNC_ADJ:  handle_sync_adj(*msg);  break;
            default: break;
        }
    }
}

// --- right: respond to SYNC_REQ ---

void audio_node::handle_sync_req(const sync_message& req)
{
    int64_t t2 = clock_.now_us();

    sync_message resp{};
    resp.type  = msg_type_t::SYNC_RESP;
    resp.seq   = req.seq;
    resp.t1_us = req.t1_us;
    resp.t2_us = t2;
    resp.t3_us = clock_.now_us();
    comms_.send(resp);
}

// --- left: compute NTP offset, send SYNC_ADJ to right ---

void audio_node::handle_sync_resp(const sync_message& resp)
{
    int64_t t4     = clock_.now_us();
    double  offset = 0.5 * ((resp.t2_us - resp.t1_us) + (resp.t3_us - t4));
    last_offset_us_ = static_cast<int64_t>(offset);


    sync_message adj{};
    adj.type  = msg_type_t::SYNC_ADJ;
    adj.seq   = resp.seq;
    adj.t1_us = static_cast<int64_t>(offset);
    comms_.send(adj);
}

// --- right: apply PI correction to own clock ---

void audio_node::handle_sync_adj(const sync_message& adj)
{
    last_offset_us_ = adj.t1_us;
    apply_pi(static_cast<double>(adj.t1_us));
}

void audio_node::apply_pi(double offset_us)
{
    // Phase correction: directly reduce clock offset
    clock_.offset_us -= static_cast<int64_t>(Kp * offset_us);
    // Frequency correction: steer ppm toward zero by an amount proportional to the current error
    clock_.ppm_error -= Kf * offset_us;
    pi_integral_     += offset_us * dt;  // kept for diagnostics
}
