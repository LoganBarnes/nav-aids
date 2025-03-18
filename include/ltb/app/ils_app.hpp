#pragma once

// project
#include "ltb/app/app.hpp"
#include "ltb/ogl/framebuffer.hpp"
#include "ltb/utils/initializable.hpp"

// generated
#include "ltb/ltb_config.hpp"

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

    ogl::Uniform< glm::vec2 > field_size_pixels_uniform_ = { program_, "field_size_pixels" };

    ogl::VertexArray vertex_array_  = { };
};

} // namespace ltb::app
