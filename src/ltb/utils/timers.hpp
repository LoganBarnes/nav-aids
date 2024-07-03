// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "duration.hpp"

// external
#include <spdlog/spdlog.h>

// standard
#include <chrono>
#include <functional>

namespace ltb::utils
{

class Timer
{
public:
    Timer( );

    auto start( ) -> void;
    auto duration_since_start( ) -> utils::Duration;

private:
    std::chrono::time_point< std::chrono::steady_clock > start_time_;
};

class ScopedTimer
{
public:
    using Callback = std::function< void( utils::Duration ) >;

    explicit ScopedTimer( Callback callback );
    ~ScopedTimer( );

private:
    Timer    timer_;
    Callback callback_;
};

} // namespace ltb::utils
