#pragma once

// project
#include "ltb/ogl/framebuffer.hpp"
#include "ltb/ogl/opengl_loader.hpp"
#include "ltb/window/window.hpp"

namespace ltb::ils
{

/// \brief The main application window for the HUD app.
class App
{
public:
    explicit App( window::Window& window );

    auto initialize( ) -> utils::Result< App* >;
    auto run( ) -> utils::Result< void >;

private:
    // Window & graphics interfaces
    window::Window& window_;
    // graphics::GraphicsFramework2d& graphics_;

    glm::ivec2 framebuffer_size_ = { };

    ogl::OpenglLoader ogl_loader_    = { };
    ogl::Texture      color_texture_ = { };
    ogl::Framebuffer  framebuffer_   = { };

    auto on_resize( ) const -> void;
    auto render_to_framebuffer( ) const -> void;
};

} // namespace ltb::ils
