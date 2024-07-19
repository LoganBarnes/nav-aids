#pragma once

// project
#include "ltb/gui/imgui_setup.hpp"
#include "ltb/ogl/framebuffer.hpp"
#include "ltb/ogl/opengl_loader.hpp"
#include "ltb/ogl/pipeline.hpp"
#include "ltb/window/window.hpp"

namespace ltb::app
{

/// \brief The main application window for the ILS app.
class IlsApp
{
public:
    explicit IlsApp( window::Window& window, gui::ImguiSetup& imgui_setup );

    auto initialize( ) -> utils::Result< IlsApp* >;
    auto run( ) -> utils::Result< void >;

private:
    // Window & graphics interfaces
    window::Window&  window_;
    gui::ImguiSetup& imgui_setup_;

    glm::ivec2 framebuffer_size_ = { };

    ogl::OpenglLoader ogl_loader_ = { };

    // OpenGL rendering objects
    ogl::Pipeline< ogl::Attributes<>, ogl::Uniforms<> > pipeline_ = { };

    // OpenGL display objects
    ogl::Texture     color_texture_ = { };
    ogl::Framebuffer framebuffer_   = { };

    auto on_resize( ) const -> void;
    auto render_to_framebuffer( ) const -> void;
    auto render_gui( ) const -> void;
};

} // namespace ltb::app
