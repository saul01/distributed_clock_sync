#include "udp_socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

udp_socket::udp_socket(uint16_t bind_port) {
    fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_ < 0) throw std::runtime_error("socket");

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(bind_port);

    if (::bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        throw std::runtime_error("bind");
}

udp_socket::~udp_socket() { ::close(fd_); }

void udp_socket::send_to(const char* host, uint16_t port,
                          const void* data, size_t len) {
    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = ::inet_addr(host);
    addr.sin_port        = htons(port);
    ::sendto(fd_, data, len, 0,
             reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
}

int udp_socket::recv(void* buf, size_t len, int timeout_ms) {
    timeval tv{};
    if (timeout_ms >= 0) {
        tv.tv_sec  = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        ::setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    } else {
        // block indefinitely — clear any previously set timeout
        ::setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    }
    return static_cast<int>(::recvfrom(fd_, buf, len, 0, nullptr, nullptr));
}
