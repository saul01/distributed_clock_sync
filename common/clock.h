#pragma once

#include <cstdint>
#include <chrono>


class sim_clock
{
public:
    sim_clock() : ppm_error(0.0), offset_us(0), start_(std::chrono::steady_clock::now()) {}
    double ppm_error;
    int64_t offset_us;

    int64_t now_us() const;

private:

    std::chrono::steady_clock::time_point start_;
};

