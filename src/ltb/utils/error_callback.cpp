#include "ltb/utils/error_callback.hpp"

// external
#include <spdlog/spdlog.h>

namespace ltb::utils
{

auto log_error( Error const& error ) -> void
{
    if ( error.severity( ) == Error::Severity::Warning )
    {
        spdlog::warn( "{}", error.debug_error_message( ) );
    }
    else
    {
        spdlog::error( "{}", error.debug_error_message( ) );
    }
}

} // namespace ltb::utils
