# AirPods-Inspired Distributed Clock Synchronization Project (Compact Context)

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

# Program Management
1. Set Bazel workspace to compile `left` and `right` with simple `main()` function that prints `hello left` and `hello right`.
2. Set gihub repo