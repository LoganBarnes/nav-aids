#pragma once

// project
#include "ltb/app/app.hpp"
#include "ltb/gui/imgui_setup.hpp"
#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/framebuffer.hpp"
#include "ltb/ogl/program.hpp"
#include "ltb/ogl/shader.hpp"
#include "ltb/ogl/uniform.hpp"
#include "ltb/ogl/vertex_array.hpp"
#include "ltb/window/window.hpp"

namespace ltb::app
{

class AntennaApp : public App
{
public:
    AntennaApp( )           = default;
    ~AntennaApp( ) override = default;

    auto initialize( glm::ivec2 framebuffer_size ) -> utils::Result< void > override;
    auto render( ) -> void override;
    auto configure_gui( ) -> void override;
    auto destroy( ) -> void override;

    auto resize( glm::ivec2 framebuffer_size ) -> void override;

private:
    ogl::Shader< GL_VERTEX_SHADER >   wave_vertex_shader_   = { };
    ogl::Shader< GL_FRAGMENT_SHADER > wave_fragment_shader_ = { };
    ogl::Program                      wave_program_         = { };

    ogl::Shader< GL_VERTEX_SHADER >   antenna_vertex_shader_   = { };
    ogl::Shader< GL_FRAGMENT_SHADER > antenna_fragment_shader_ = { };
    ogl::Program                      antenna_program_         = { };

    ogl::Uniform< glm::mat4 > projection_from_world_uniform_
        = ogl::Uniform< glm::mat4 >{ antenna_program_ };
    ogl::Uniform< float32 > time_uniform_ = ogl::Uniform< float32 >{ antenna_program_ };

    struct Antenna
    {
        glm::vec2 world_position;
        float32   antenna_power;
    };

    std::vector< Antenna > antennas_ = { };

    ogl::Buffer      antenna_vertex_buffer_ = { };
    ogl::VertexArray antenna_vertex_array_  = { };

    static constexpr auto framebuffer_count_ = 2UZ;

    std::array< ogl::Texture, framebuffer_count_ >     wave_field_textures_     = { };
    std::array< ogl::Framebuffer, framebuffer_count_ > wave_field_framebuffers_ = { };
    uint32                                             current_wave_field_      = 0;

    auto draw( ) -> utils::Result< void >;
};

} // namespace ltb::app
