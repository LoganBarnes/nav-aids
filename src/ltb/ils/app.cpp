#include "ltb/ils/app.hpp"

// external
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

namespace ltb::ils
{

App::App( window::FullscreenWindow& window )
    : window_( window )
{
}

auto App::initialize( ) -> utils::Result< App* >
{
    LTB_CHECK( auto const framebuffer_size, window_.initialize( "Instrument Landing System" ) );
    spdlog::info( "Framebuffer size: {}x{}", framebuffer_size.x, framebuffer_size.y );

    LTB_CHECK( ogl_loader_.initialize( ) );
    LTB_CHECK( texture_.initialize( ) );

    return this;
}

auto App::run( ) -> utils::Result< void >
{
    while ( !window_.should_close( ) )
    {
        window_.poll_events( );

        /// \todo(logan): Render the scene here.

        window_.swap_buffers( );
    }

    return utils::success( );
}

} // namespace ltb::ils
