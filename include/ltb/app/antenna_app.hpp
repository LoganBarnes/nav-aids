#pragma once

// project
#include "ltb/app/app.hpp"
#include "ltb/gui/imgui_setup.hpp"
#include "ltb/ogl/framebuffer_chain.hpp"
#include "ltb/ogl/pipeline.hpp"
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
    std::chrono::steady_clock::time_point start_time_ = { };

    // Framebuffers and textures to store the wave field.
    static constexpr auto framebuffer_count_ = 3UZ;

    glm::ivec2                                  framebuffer_size_ = { };
    ogl::FramebufferChain< framebuffer_count_ > wave_field_chain_ = { };

    // Program to propagate the wave field.
    ogl::Pipeline< glm::vec2, ogl::Texture, ogl::Texture > wave_pipeline_ = { };

    // Program to set antenna positions and strength.
    ogl::Pipeline< glm::mat4, float32, float32 > antenna_pipeline_ = { };

    struct Antenna
    {
        glm::vec2 world_position;
        float32   antenna_power;
    };

    std::vector< Antenna > antennas_ = { };

    // Program to display the wave field.
    ogl::Pipeline< ogl::Texture > display_pipeline_ = { };

    auto update_framebuffer( ) -> void;
    auto propagate_waves( ) -> void;
    auto render_antennas( ) -> void;
    auto display_wave_field( ) -> void;
};

} // namespace ltb::app
