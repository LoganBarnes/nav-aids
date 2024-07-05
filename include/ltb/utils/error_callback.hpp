// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "error.hpp"

// standard
#include <functional>
#include <stdexcept>

namespace ltb::utils
{

auto log_error( Error const& error ) -> void;

} // namespace ltb::utils