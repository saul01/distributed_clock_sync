#pragma once
#include <cstddef>
#include <cstdint>

class udp_socket {
public:
    explicit udp_socket(uint16_t bind_port);
    ~udp_socket();

    void send_to(const char* host, uint16_t port, const void* data, size_t len);
    // Returns bytes received, -1 on timeout or error.
    // timeout_ms < 0 → block indefinitely; 0 → non-blocking; >0 → wait that many ms.
    int  recv(void* buf, size_t len, int timeout_ms = -1);

private:
    int fd_;
};
