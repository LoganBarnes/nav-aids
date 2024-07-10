
// project
#include "ltb/app/vor_app.hpp"
#include "ltb/gui/glfw_opengl_imgui_setup.hpp"
#include "ltb/window/glfw_window.hpp"

// external
#include <spdlog/spdlog.h>

namespace ltb
{
namespace
{

auto app_main( ) -> utils::Result< void >
{
    auto window = window::GlfwWindow{ };
    auto imgui  = gui::GlfwOpenglImguiSetup{ window };

    auto app = app::VorApp{ window, imgui };

    return app.initialize( ).and_then( &app::VorApp::run );
}

} // namespace
} // namespace ltb

auto main( ) -> int
{
    spdlog::set_level( spdlog::level::debug );

    if ( auto result = ltb::app_main( ); !result )
    {
        spdlog::error( "{}", result.error( ).error_message( ) );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
