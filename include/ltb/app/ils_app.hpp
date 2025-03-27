#pragma once

// project
#include "ltb/app/app.hpp"
#include "ltb/ogl/framebuffer.hpp"
#include "ltb/utils/initializable.hpp"

// generated
#include "ltb/ltb_config.hpp"

// standard
#include <chrono>

namespace ltb::app
{

/// \brief The main application window for the ILS app.
class IlsApp : public App
{
public:
    IlsApp( )           = default;
    ~IlsApp( ) override = default;

    auto initialize( glm::ivec2 framebuffer_size ) -> utils::Result< void > override;
    auto render( ) -> void override;
    auto configure_gui( ) -> void override;
    auto destroy( ) -> void override;

    auto resize( glm::ivec2 framebuffer_size ) -> void override;

private:
    glm::ivec2 framebuffer_size_ = { };

    ogl::Shader< GL_VERTEX_SHADER > fullscreen_vertex_shader_ = {
        config::shader_dir_path( ) / "fullscreen.vert",
    };
    ogl::Shader< GL_FRAGMENT_SHADER > ils_fragment_shader_ = {
        config::shader_dir_path( ) / "ils.frag",
    };
    ogl::Program program_ = { fullscreen_vertex_shader_, ils_fragment_shader_ };

    ogl::Uniform< float32 >   pixel_size_m_uniform_      = { program_, "pixel_size_m" };
    ogl::Uniform< int32 >     antenna_pairs_uniform_     = { program_, "antenna_pairs" };
    ogl::Uniform< float32 >   antenna_spacing_m_uniform_ = { program_, "antenna_spacing_m" };
    ogl::Uniform< glm::vec3 > output_scale_uniform_      = { program_, "output_scale" };
    ogl::Uniform< float32 >   time_s_uniform_            = { program_, "time_s" };
    ogl::Uniform< glm::vec2 > field_size_pixels_uniform_ = { program_, "field_size_pixels" };

    ogl::VertexArray vertex_array_ = { };

    std::chrono::steady_clock::time_point start_time_   = { };
    float32                               time_scale_s_ = 0.0F;

    float32   pixel_size_m      = 1.0F;
    int32     antenna_pairs_    = 1;
    float32   antenna_spacing_m = 5.0F;
    glm::vec3 output_channels_  = { 1.0F, 1.0F, 1.0F };
    float32   output_scale_     = 0.1F;

    enum class Display
    {
        CSB,
        SBO,
        Both
    };
    Display display_ = Display::Both;
};

} // namespace ltb::app
