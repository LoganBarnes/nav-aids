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

// external
#include <implot.h>

namespace ltb::app
{

/// \brief The main application window for the VOR app.
class VorApp
{
public:
    explicit VorApp( window::Window& window, gui::ImguiSetup& imgui_setup );

    auto initialize( ) -> utils::Result< VorApp* >;
    auto run( ) -> utils::Result< void >;

private:
    // Window & graphics interfaces
    window::Window&                  window_;
    gui::ImguiSetup&                 imgui_setup_;
    std::shared_ptr< ImPlotContext > implot_context_ = nullptr;

    ogl::OpenglLoader ogl_loader_ = { };

    std::vector< float64 > time_ms_x_values_ = { };

    float64 carrier_frequency_mhz_       = 117.3F;
    float64 carrier_frequency_period_ms_ = { };

    std::vector< float64 > carrier_wave_y_values_         = { };
    std::vector< float64 > reference_audio_wave_y_values_ = { };
    std::vector< float64 > composite_radio_wave_y_values_ = { };

    auto render_gui( ) -> void;
    auto update_frequencies( ) -> void;
};

} // namespace ltb::app
