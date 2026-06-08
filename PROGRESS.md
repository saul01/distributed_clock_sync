# Distributed Clock Sync — Progress

## Goal
Simulate how two wireless earbuds (left, right) maintain a shared notion of time for
synchronized audio playback. Demonstrates C++, UDP sockets, distributed systems,
clock synchronization, and PLL/FLL control loops.

---

## Completed

### Bazel workspace
- `MODULE.bazel` + `rules_cc` configured.
- `bazel build //left:left //right:right` and `bazel run` work cleanly.

### Simulated clock (`common/clock.h`, `common/clock.cpp`)
- `sim_clock(float ppm_error, int64_t offset_us)` models a hardware oscillator
  with a configurable frequency error (ppm) and initial offset.
- `now_us()` returns a monotonic timestamp in microseconds, scaled by the ppm drift.
- left: `ppm=0, offset=0` (reference clock).
- right: `ppm=+20, offset=5000 µs` (drifting clock).

### UDP transport (`common/udp_socket.h`, `common/udp_socket.cpp`, `common/msg.h`)
- `udp_socket` wraps a POSIX SOCK_DGRAM socket with `send_to` / `recv`.
- `ts_msg` carries `node_id` + `ts_us` between peers.
- left binds port 9001, right binds port 9002, both on localhost.
- Each process spawns a receive thread and sends its timestamp every second.
- Confirmed: timestamps cross the wire and drift is visible
  (right's timestamps consistently ~5000 µs ahead of left's plus growing ppm offset).

---

## Next Steps

- **Clock-offset estimation** — on each received timestamp, compute the one-way
  delay and offset between the two clocks (NTP-style or PTP).
- **PLL/FLL control loop** — feed the offset estimate into a proportional-integral
  controller that steers `ppm_error` and `offset_us` on the follower to converge
  toward the reference.
- **Convergence logging** — plot offset error over time to show the loop locking.
- **Multi-machine** — replace localhost with real network addresses to run left and
  right on separate machines or VMs.
