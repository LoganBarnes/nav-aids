#include "ltb/ils/app.hpp"

// external
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

namespace ltb::ils
{

App::App( window::Window& window )
    : window_( window )
{
}

auto App::initialize( ) -> utils::Result< App* >
{
    LTB_CHECK(
        framebuffer_size_,
        window_.initialize( {
            .title        = "Instrument Landing System",
            .initial_size = glm::ivec2{ 1280, 720 },
        } )
    );
    spdlog::info( "Framebuffer size: {}x{}", framebuffer_size_.x, framebuffer_size_.y );

    LTB_CHECK( ogl_loader_.initialize( ) );
    LTB_CHECK( texture_.initialize( ) );

    return this;
}

auto App::run( ) -> utils::Result< void >
{
    while ( !window_.should_close( ) )
    {
        window_.poll_events( );

        if ( auto const resized = window_.resized( ) )
        {
            framebuffer_size_ = resized.value( );
            spdlog::info( "Resized to {}x{}", framebuffer_size_.x, framebuffer_size_.y );
        }

        /// \todo(logan): Render the scene here.

        glViewport( 0, 0, framebuffer_size_.x, framebuffer_size_.y );
        glClear( GL_COLOR_BUFFER_BIT );

        window_.swap_buffers( );
    }

    return utils::success( );
}

} // namespace ltb::ils
