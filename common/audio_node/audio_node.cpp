#include "audio_node.h"
#include <iostream>

audio_node::audio_node(uint16_t own_port, uint16_t peer_port)
    : comms_(own_port, peer_port), clock_(0.0f, 0)
{
}

audio_node::~audio_node()
{
    running_ = false;
    if (rx_thread_.joinable())
        rx_thread_.join();
}

void audio_node::start()
{
    sync_message msg{};
    msg.type = msg_type_t::START;
    comms_.send(msg);
    std::cout << "[left]  sent START" << std::endl;

    running_ = true;
    rx_thread_ = std::thread(&audio_node::run_rx_loop, this);
}

void audio_node::wait_for_start()
{
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

bool audio_node::is_running() const
{
    return running_;
}

void audio_node::run_rx_loop()
{
    while (running_) {
        auto msg = comms_.receive(500);
        if (!msg) continue;

        std::cout << "[node] rx type=" << static_cast<int>(msg->type)
                  << " seq=" << msg->seq
                  << " t1=" << msg->t1_us << " us" << std::endl;
    }
}
