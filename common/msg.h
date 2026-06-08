#pragma once
#include <cstdint>

struct ts_msg {
    uint8_t  node_id;   // 0=left, 1=right
    uint8_t  pad[7];
    int64_t  ts_us;
};
