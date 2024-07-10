#pragma once

// project
#include "ltb/gui/imgui_setup.hpp"
#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/framebuffer.hpp"
#include "ltb/ogl/opengl_loader.hpp"
#include "ltb/ogl/program.hpp"
#include "ltb/ogl/shader.hpp"
#include "ltb/ogl/vertex_array.hpp"
#include "ltb/window/window.hpp"

namespace ltb::app
{

/// \brief The main application window for the HUD app.
class VorApp
{
public:
    explicit VorApp( window::Window& window, gui::ImguiSetup& imgui_setup );

    auto initialize( ) -> utils::Result< VorApp* >;
    auto run( ) -> utils::Result< void >;

private:
    // Window & graphics interfaces
    window::Window&  window_;
    gui::ImguiSetup& imgui_setup_;

    ogl::OpenglLoader ogl_loader_ = { };

    float                base_frequency_mhz_          = 117.3F;
    float                base_frequency_wavelength_m_ = { };
    std::vector< float > base_frequency_values_       = { };

    auto render_gui( ) -> void;
};

} // namespace ltb::app
