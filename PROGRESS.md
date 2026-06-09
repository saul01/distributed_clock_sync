# Distributed Clock Sync — Progress

## Goal
Simulate how two wireless earbuds (left, right) maintain a shared notion of time for
synchronized audio playback. Demonstrates C++, UDP sockets, distributed systems,
clock synchronization, and PLL/FLL control loops.

---

## Completed

### Bazel workspace
- `MODULE.bazel` + `rules_cc` configured.
- `.bazelrc` sets `--cxxopt=-std=c++17`.
- `bazel build //left:left //right:right` builds cleanly.

### Simulated clock (`common/clock/clock.h`, `common/clock/clock.cpp`)
- `sim_clock(float ppm_error, int64_t offset_us)` models a hardware oscillator
  with a configurable frequency error (ppm) and initial offset.
- `now_us()` returns a monotonic timestamp in microseconds, scaled by ppm drift.
- Both nodes currently start at `ppm=0, offset=0`; parameters are easy to change.

### Comms layer (`common/comms/`)
- `i_comms` — abstract interface: `send(sync_message)`, `receive(timeout_ms) → optional<sync_message>`.
- `udp_comms(bind_port, peer_port)` — POSIX UDP socket; always sends to `127.0.0.1:peer_port`.
- `sync_message` — POD struct: `msg_type_t type`, `seq`, `t1_us`, `t2_us`, `t3_us`.
- `msg_type_t`: `START=1`, `SYNC_REQ=2`, `SYNC_RESP=3`.

### audio_node (`common/audio_node/`)
- Owns `udp_comms`, `sim_clock`, and a background rx thread.
- `start()` — sends `START` to peer, spawns rx loop thread.
- `wait_for_start()` — blocks until `START` is received, then spawns rx loop thread.
- `is_running()` — returns whether rx thread is live.
- START handshake confirmed working end-to-end.

### Processes
- `left/main.cpp` — `audio_node(9001, 9002)` → `start()`
- `right/main.cpp` — `audio_node(9002, 9001)` → `wait_for_start()`

---

## Next Steps

1. **SYNC_REQ/SYNC_RESP exchange** — left periodically sends `SYNC_REQ` with `t1=clock.now_us()`;
   right records `t2`, replies with `SYNC_RESP` carrying `t1, t2, t3`.
2. **Offset estimation** — left computes `offset = ((t2-t1) + (t3-t4)) / 2` (NTP 4-timestamp formula).
3. **PI control loop** — feed offset into a proportional-integral controller that steers
   right's `ppm_error` and `offset_us` toward zero over time.
4. **Convergence logging** — log offset error per sync round to show the loop locking in.
5. **Multi-machine** — replace `127.0.0.1` with configurable peer address for cross-machine testing.
