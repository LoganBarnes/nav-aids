// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////

// project
#include "app.hpp"
#include "ltb/exec/app_main.hpp"

auto main( ltb::int32 const argc, char const* argv[] ) -> int
{
    spdlog::set_level( spdlog::level::info );
    return ltb::exec::windowed_app_main< ltb::IlsApp >( argc, argv );
}
