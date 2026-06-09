#include "clock.h"

using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::steady_clock;
using std::chrono::system_clock;

sim_clock::sim_clock(float _ppm, int64_t _off_us)
{
    ppm_error = _ppm;
    offset_us = _off_us;
    start_    = steady_clock::now();
    // Absolute wall-clock epoch shared by all processes on the same machine.
    start_unix_us_ = duration_cast<microseconds>(
        system_clock::now().time_since_epoch()).count();
}

int64_t sim_clock::now_us() const
{
    // Elapsed real time since this clock was constructed.
    auto elapsed_us =
        duration_cast<microseconds>(steady_clock::now() - start_).count();

    // Ppm drift is applied only to elapsed time, not to the absolute epoch,
    // so the drift rate is correct regardless of the Unix timestamp magnitude.
    double drift_us = elapsed_us * (ppm_error / 1'000'000.0);

    return start_unix_us_ + offset_us + elapsed_us + static_cast<int64_t>(drift_us);
}
