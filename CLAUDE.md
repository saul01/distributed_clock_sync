# AirPods-Inspired Distributed Clock Synchronization Project (Compact Context)

# See Also
Read PROGRESS.md for detailed status and next steps.

# Goal
Build a firmware-style project that simulates how two wireless earbuds maintain a shared notion of time for synchronized audio playback.
The project demonstrates:
C++
UDP sockets
Distributed systems
Clock synchronization
PLL/FLL control loops
Real-time thinking
Audio timestamp scheduling
This is a strong interview project for Apple AirPods, NVIDIA AV, embedded firmware, RTOS, and systems software roles.

# Build System
Bazel

# High Level Architecture
left
right

# Initially on the same Mac
MacBook
```
+---------+
|  left   |
+---------+

UDP localhost

+---------+
|  right  |
+---------+
```

# Simulated Clock
```
class SimulatedClock
{
public:
    double ppm_error;
    int64_t offset_us;
};
```
# Example
```Left:
offset = 0 us
ppm = 0

Right:
offset = 5000 us
ppm = +20
```
# Rules
1. Source files end with .cpp, .c, .h, .py
2. use sneak case

# Program Management
1. Set Bazel workspace to compile `left` and `right` with simple `main()` function that prints `hello left` and `hello right`.
2. Set gihub repo
3. Add UDP 

# Completed
1. git repo
2. Bazel workspace (`MODULE.bazel`, `rules_cc`)
3. `left` and `right` processes
4. `sim_clock` — wall-clock epoch + ppm drift on elapsed time; `now_us()` returns consistent absolute timestamps across processes
5. `udp_comms` — POSIX UDP socket, `send(sync_message)` targets `127.0.0.1:peer_port`, `receive(timeout_ms)` returns `std::optional<sync_message>`
6. `audio_node` — owns udp_comms + sim_clock + rx thread + sync thread; START handshake, then NTP-style SYNC_REQ/SYNC_RESP/SYNC_ADJ loop
7. NTP offset estimation: `offset = ((t2-t1) + (t3-t4)) / 2`; left sends SYNC_ADJ to right with computed offset
8. PI-style controller on right: phase correction (Kp=0.5) + frequency correction (Kf=0.002); converges 5000 µs / 20 ppm to ±5 µs / ~0 ppm in ~12 rounds (2.4 s)

# Architecture (current)
```
common/clock/       sim_clock (clock.h, clock.cpp)
common/comms/       i_comms interface, udp_comms (udp_comms.h, udp_comms.cpp, i_comms.h)
common/audio_node/  audio_node (audio_node.h, audio_node.cpp)
common/BUILD        targets: :clock, :comms, :audio_node
left/main.cpp       audio_node(LEFT_PORT=9001, RIGHT_PORT=9002, ppm=0, off=0); start()
right/main.cpp      audio_node(RIGHT_PORT=9002, LEFT_PORT=9001, ppm=20, off=5000); wait_for_start()
.bazelrc            --cxxopt=-std=c++17
```

# Message types (common/comms/i_comms.h)
- `msg_type_t::START` (1) — sent by left to kick off session
- `msg_type_t::SYNC_REQ` (2) — left→right; carries t1 (left timestamp at send)
- `msg_type_t::SYNC_RESP` (3) — right→left; carries t1, t2, t3
- `msg_type_t::SYNC_ADJ` (4) — left→right; t1_us = computed offset estimate (µs); right applies PI
- `sync_message` struct: type, _pad[7], seq, t1_us, t2_us, t3_us

# Next Task
1. **Convergence plot** — log offset/ppm to CSV and plot with Python to show lock-in curve
2. **Multi-machine** — replace hardcoded `127.0.0.1` with configurable peer address
3. **Audio scheduling** — use synchronized clock to schedule a tone at a shared absolute timestamp, verifying both nodes play in sync