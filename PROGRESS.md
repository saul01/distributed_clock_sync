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
- `sim_clock(float ppm_error, int64_t offset_us)` models a hardware oscillator.
- `now_us()` returns an absolute timestamp: wall-clock epoch at construction + elapsed
  time scaled by ppm drift + offset. Both processes share the same epoch (system clock),
  so NTP offset measurements reflect only the configured drift/offset, not startup lag.
- `ppm_error` and `offset_us` are public — the PI controller writes to them directly.

### Comms layer (`common/comms/`)
- `i_comms` — abstract interface: `send(sync_message)`, `receive(timeout_ms) → optional<sync_message>`.
- `udp_comms(bind_port, peer_port)` — POSIX UDP socket; always sends to `127.0.0.1:peer_port`.
- `sync_message` — POD struct: `msg_type_t type`, `seq`, `t1_us`, `t2_us`, `t3_us`.
- `msg_type_t`: `START=1`, `SYNC_REQ=2`, `SYNC_RESP=3`, `SYNC_ADJ=4`.

### audio_node (`common/audio_node/`)
- Owns `udp_comms`, `sim_clock`, rx thread, and (left only) sync thread.
- `start()` — sends `START`, spawns rx loop + periodic SYNC_REQ sender (200 ms).
- `wait_for_start()` — blocks until `START` received, spawns rx loop.
- **NTP-style exchange** (every 200 ms):
  1. left sends `SYNC_REQ` with `t1 = left.now_us()`
  2. right records `t2`, replies `SYNC_RESP(t1, t2, t3)`
  3. left records `t4`, computes `offset = ((t2−t1) + (t3−t4)) / 2`
  4. left sends `SYNC_ADJ` to right (offset in `t1_us`)
  5. right applies PI: `offset_us -= Kp * offset`, `ppm_error -= Kf * offset`

### PI-style controller
- `Kp = 0.5` — phase: halves the clock offset each sync round.
- `Kf = 0.002` — frequency: reduces ppm_error proportional to current offset.
- **Measured convergence** (right starts at ppm=20, offset=5000 µs):
  - seq 0: offset ≈ 5039 µs, ppm ≈ 9.9
  - seq 6: offset ≈ 85 µs, ppm ≈ 0.06
  - seq 12: offset ≈ 0 µs, ppm ≈ −0.1 (locked)
  - Steady-state jitter: ±5 µs (localhost UDP noise floor)

### Processes
- `left/main.cpp` — `audio_node(9001, 9002, ppm=0, off=0)` → `start()`
- `right/main.cpp` — `audio_node(9002, 9001, ppm=20, off=5000)` → `wait_for_start()`

---

## Next Steps

1. **Convergence plot** — log offset/ppm per round to CSV; plot with Python to visualize lock-in.
2. **Multi-machine** — replace `127.0.0.1` with configurable peer address for cross-machine testing.
3. **Audio scheduling** — schedule a shared tone at an absolute timestamp using the synchronized
   clock; verify both nodes fire at the same real-world moment.
