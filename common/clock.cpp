#include "clock.h"

using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::steady_clock;

int64_t sim_clock::now_us() const
{
    auto elapsed_us =
        duration_cast<microseconds>(
            steady_clock::now() - start_)
            .count();

    double scaled =
        elapsed_us * (1.0 + ppm_error / 1'000'000.0);

    return offset_us +
           static_cast<int64_t>(scaled);
}