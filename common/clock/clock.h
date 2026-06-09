#pragma once

#include <cstdint>
#include <chrono>


class sim_clock
{
public:
    sim_clock(float _ppm, int64_t _off_us);
    double  ppm_error;
    int64_t offset_us;

    int64_t now_us() const;

private:
    // Wall-clock microseconds at construction — shared epoch across both processes.
    int64_t                                start_unix_us_;
    std::chrono::steady_clock::time_point  start_;
};

