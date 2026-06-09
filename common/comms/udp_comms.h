#pragma once
#include <cstdint>
#include <optional>
#include "i_comms.h"

class udp_comms : public i_comms {
public:
    udp_comms(uint16_t bind_port, uint16_t peer_port);
    ~udp_comms();

    bool send(const sync_message& msg) override;
    std::optional<sync_message> receive(int timeout_ms) override;

private:
    int      fd_;
    uint16_t peer_port_;
};
