#pragma once

// project
#include "ltb/ogl/opengl_loader.hpp"
#include "ltb/ogl/texture.hpp"
#include "ltb/window/fullscreen_window.hpp"

namespace ltb::ils
{

/// \brief The main application window for the HUD app.
class App
{
public:
    explicit App( window::FullscreenWindow& window );

    auto initialize( ) -> utils::Result< App* >;
    auto run( ) -> utils::Result< void >;

private:
    // Window & graphics interfaces
    window::FullscreenWindow& window_;
    // graphics::GraphicsFramework2d& graphics_;

    ogl::OpenglLoader ogl_loader_ = { };
    ogl::Texture      texture_    = { };
};

} // namespace ltb::ils
