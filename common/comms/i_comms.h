#pragma once

#include <cstdint>
#include <optional>

enum class msg_type_t : uint8_t
{
    START     = 1,
    SYNC_REQ  = 2,
    SYNC_RESP = 3
};

struct sync_message
{
    msg_type_t type;
    uint8_t    _pad[7]{};
    uint64_t   seq{0};
    int64_t    t1_us{0};
    int64_t    t2_us{0};
    int64_t    t3_us{0};
};

class i_comms
{
public:
    virtual ~i_comms() = default;
    virtual bool send(const sync_message& msg) = 0;
    virtual std::optional<sync_message> receive(int timeout_ms) = 0;
};
