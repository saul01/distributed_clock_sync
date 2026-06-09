#include "udp_comms.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

udp_comms::udp_comms(uint16_t bind_port, uint16_t peer_port)
    : peer_port_(peer_port)
{
    fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_ < 0) throw std::runtime_error("socket");

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(bind_port);

    if (::bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        throw std::runtime_error("bind");
}

udp_comms::~udp_comms() { ::close(fd_); }

bool udp_comms::send(const sync_message& msg)
{
    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
    addr.sin_port        = htons(peer_port_);

    ssize_t sent = ::sendto(fd_, &msg, sizeof(msg), 0,
                            reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    return sent == static_cast<ssize_t>(sizeof(msg));
}

std::optional<sync_message> udp_comms::receive(int timeout_ms)
{
    if (timeout_ms >= 0) {
        timeval tv{};
        tv.tv_sec  = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        ::setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    }

    sync_message msg{};
    ssize_t n = ::recvfrom(fd_, &msg, sizeof(msg), 0, nullptr, nullptr);
    if (n != static_cast<ssize_t>(sizeof(msg)))
        return std::nullopt;
    return msg;
}
