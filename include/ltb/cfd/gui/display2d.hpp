#pragma once

// project
#include "ltb/ogl/program_attribute.hpp"
#include "ltb/ogl/program_uniform.hpp"
#include "ltb/ogl/vertex_array.hpp"

// generated
#include "ltb/ltb_config.hpp"

namespace ltb::cfd
{

class CfdDisplay2d
{
public:
    CfdDisplay2d( ) = default;

    auto initialize( glm::ivec2 framebuffer_size ) -> utils::Result< void >;
    auto render( ) -> void;

    auto resize( glm::ivec2 framebuffer_size ) -> void;

private:
    ogl::Shader< GL_VERTEX_SHADER > vertex_shader = {
        config::shader_dir_path( ) / "cfd" / "display2d.vert",
    };
    ogl::Shader< GL_FRAGMENT_SHADER > fragment_shader = {
        config::shader_dir_path( ) / "cfd" / "display2d.frag",
    };
    ogl::Program program = { vertex_shader, fragment_shader };

    ogl::Uniform< ogl::Texture > wave_texture_uniform     = { program, "wave_texture" };
    ogl::Uniform< glm::vec2 >    framebuffer_size_uniform = { program, "framebuffer_size" };

    ogl::Buffer      vertex_buffer = { };
    ogl::VertexArray vertex_array  = { };
};

} // namespace ltb::cfd
