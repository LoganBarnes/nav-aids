
// project
#include "ltb/app/antenna_app.hpp"
#include "ltb/app/app.hpp"
#include "ltb/app/cfd_lesson_1_app.hpp"
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

    auto app_runner = app::AppRunner{
        window,
        imgui,
        {
            { "CFD Lesson 1", std::make_shared< app::CfdLesson1App >( ) },
            { "Antenna", std::make_shared< app::AntennaApp >( ) },
        }
    };

    return app_runner.initialize( ).and_then( &app::AppRunner::run );
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
