// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Rotor Technologies, Inc. - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////

// project
#include "ltb/ils/app.hpp"
#include "ltb/window/glfw_window.hpp"

// external
#include <spdlog/spdlog.h>

namespace ltb
{
namespace
{

auto rotor_main( ) -> utils::Result< void >
{
    auto window = window::GlfwWindow{ };

    auto app = ils::App{ window };

    return app.initialize( ).and_then( &ils::App::run );
}

} // namespace
} // namespace ltb

auto main( ) -> int
{
    spdlog::set_level( spdlog::level::info );

    if ( auto result = ltb::rotor_main( ); !result )
    {
        spdlog::error( "{}", result.error( ).error_message( ) );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
