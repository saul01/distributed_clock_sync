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
4. `sim_clock` — models ppm drift + offset; `now_us()` returns scaled microsecond timestamp
5. `udp_comms` — POSIX UDP socket, `send(sync_message)` targets `127.0.0.1:peer_port`, `receive(timeout_ms)` returns `std::optional<sync_message>`
6. `audio_node` — owns udp_comms + sim_clock + rx thread; `start()` sends START to peer; `wait_for_start()` blocks until START received; both then run rx loop
7. START handshake confirmed working: left sends START → right receives it, both enter rx loop

# Architecture (current)
```
common/clock/       sim_clock (clock.h, clock.cpp)
common/comms/       i_comms interface, udp_comms (udp_comms.h, udp_comms.cpp, i_comms.h)
common/audio_node/  audio_node (audio_node.h, audio_node.cpp)
common/BUILD        targets: :clock, :comms, :audio_node
left/main.cpp       audio_node(LEFT_PORT=9001, RIGHT_PORT=9002); start()
right/main.cpp      audio_node(RIGHT_PORT=9002, LEFT_PORT=9001); wait_for_start()
.bazelrc            --cxxopt=-std=c++17
```

# Message types (common/comms/i_comms.h)
- `msg_type_t::START` (1) — sent by left to kick off session
- `msg_type_t::SYNC_REQ` (2) — will carry t1 timestamp
- `msg_type_t::SYNC_RESP` (3) — will carry t1, t2, t3 timestamps
- `sync_message` struct: type, _pad[7], seq, t1_us, t2_us, t3_us

# Next Task
Implement NTP-style clock offset estimation:
1. left periodically sends SYNC_REQ with t1=left_clock.now_us()
2. right receives it, records t2=right_clock.now_us(), sends SYNC_RESP with t1+t2+t3=right_clock.now_us()
3. left receives SYNC_RESP, records t4=left_clock.now_us()
4. offset = ((t2-t1) + (t3-t4)) / 2
5. Feed offset into a PI controller to steer right's sim_clock ppm_error and offset_us toward zero