#pragma once

// project
#include "ltb/app/app.hpp"
#include "ltb/gui/imgui_setup.hpp"
#include "ltb/ogl/framebuffer_chain.hpp"
#include "ltb/ogl/pipeline.hpp"
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
    static constexpr auto framebuffer_count_ = 3UZ;

    glm::ivec2                                  framebuffer_size_ = { };
    ogl::FramebufferChain< framebuffer_count_ > wave_field_chain_ = { };

    // Program to propagate the wave field.
    // using WavePipeline = ogl::
    //     Pipeline< ogl::Attributes<>, ogl::Uniforms< glm::vec2, ogl::Texture, ogl::Texture > >;
    // WavePipeline wave_pipeline_ = {
    //     ogl::attribute_names( ),
    // };

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

        ogl::Program program = { };

        ogl::Attribute< decltype( Antenna::world_position ) > world_position_attribute
            = { program, "world_position" };
        ogl::Attribute< decltype( Antenna::antenna_power ) > antenna_power_attribute
            = { program, "antenna_power" };



        ogl::Buffer      vertex_buffer = { };
        ogl::VertexArray vertex_array  = { };
    } antenna_pipeline_;

    // using AntennaPipeline = ogl::Pipeline<
    //     ogl::Attributes< decltype( Antenna::world_position ), decltype( Antenna::antenna_power ) >,
    //     ogl::Uniforms< glm::mat4, float32, float32 > >;
    // AntennaPipeline antenna_pipeline_ = {
    //     ogl::attribute_names( "world_position", "antenna_power" ),
    // };

    std::vector< Antenna > antennas_ = { };

    // Program to display the wave field.
    // ogl::Pipeline< ogl::Attributes<>, ogl::Uniforms< ogl::Texture > > display_pipeline_ = {
    //     ogl::attribute_names( ),
    // };

    auto update_framebuffer( ) -> void;
    auto propagate_waves( ) -> void;
    auto render_antennas( ) -> void;
    auto display_wave_field( ) -> void;
};

} // namespace ltb::app
