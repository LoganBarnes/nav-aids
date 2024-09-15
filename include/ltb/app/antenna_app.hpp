#pragma once

// project
#include "ltb/app/app.hpp"
#include "ltb/gui/imgui_setup.hpp"
#include "ltb/ogl/framebuffer_chain.hpp"
#include "ltb/utils/initializable.hpp"
#include "ltb/window/window.hpp"

// generated
#include "ltb/ltb_config.hpp"

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
    std::chrono::steady_clock::time_point start_time_ = { };

    // Framebuffers and textures to store the wave field.
    static constexpr auto framebuffer_count_ = 3_UZ;

    glm::ivec2                                  framebuffer_size_ = { };
    ogl::FramebufferChain< framebuffer_count_ > wave_field_chain_ = { };

    ogl::Shader< GL_VERTEX_SHADER > fullscreen_vertex_shader_
        = { config::shader_dir_path( ) / "fullscreen.vert" };
    ogl::VertexArray fullscreen_vertex_array_ = { };

    struct WavePipeline
    {
        ogl::Shader< GL_VERTEX_SHADER >&  vertex_shader;
        ogl::Shader< GL_FRAGMENT_SHADER > fragment_shader
            = { config::shader_dir_path( ) / "wave.frag" };

        ogl::Program program = { vertex_shader, fragment_shader };

        ogl::Uniform< glm::vec2 >    state_size_uniform = { program, "state_size" };
        ogl::Uniform< ogl::Texture > prev_state_uniform = { program, "prev_state" };
        ogl::Uniform< ogl::Texture > curr_state_uniform = { program, "curr_state" };

        ogl::VertexArray& vertex_array;
    };

    WavePipeline wave_pipeline_ = {
        .vertex_shader = fullscreen_vertex_shader_,
        .vertex_array  = fullscreen_vertex_array_,
    };

    // Program to set antenna positions and strength.
    struct Antenna
    {
        glm::vec2 world_position;
        float32   antenna_power;
    };

    struct AntennaPipeline
    {
        ogl::Shader< GL_VERTEX_SHADER > vertex_shader
            = { config::shader_dir_path( ) / "antenna.vert" };
        ogl::Shader< GL_FRAGMENT_SHADER > fragment_shader
            = { config::shader_dir_path( ) / "antenna.frag" };

        ogl::Program program = { vertex_shader, fragment_shader };

        ogl::Uniform< glm::mat4 > clip_from_world_uniform = { program, "clip_from_world" };
        ogl::Uniform< float32 >   time_s_uniform          = { program, "time_s" };
        ogl::Uniform< float32 >   frequency_hz_uniform    = { program, "frequency_hz" };

        ogl::Buffer      vertex_buffer = { };
        ogl::VertexArray vertex_array  = { };
    };

    AntennaPipeline antenna_pipeline_ = { };

    std::vector< Antenna > antennas_ = { };

    struct DisplayPipeline
    {
        ogl::Shader< GL_VERTEX_SHADER >&  vertex_shader;
        ogl::Shader< GL_FRAGMENT_SHADER > fragment_shader
            = { config::shader_dir_path( ) / "wave_display.frag" };

        ogl::Program program = { vertex_shader, fragment_shader };

        ogl::Uniform< ogl::Texture > wave_texture_uniform = { program, "wave_texture" };

        ogl::VertexArray& vertex_array;
    };

    DisplayPipeline display_pipeline_ = {
        .vertex_shader = fullscreen_vertex_shader_,
        .vertex_array  = fullscreen_vertex_array_,
    };

    auto update_framebuffer( ) -> void;
    auto propagate_waves( ) -> void;
    auto render_antennas( ) -> void;
    auto display_wave_field( ) -> void;
};

} // namespace ltb::app
