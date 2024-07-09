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
