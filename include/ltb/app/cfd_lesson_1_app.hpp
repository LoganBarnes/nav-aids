#pragma once

// project
#include "ltb/app/app.hpp"
#include "ltb/cfd/cfd_options.hpp"
#include "ltb/gui/imgui_setup.hpp"
#include "ltb/gui/mesh_display_pipeline.hpp"
#include "ltb/ogl/framebuffer_chain.hpp"
#include "ltb/utils/initializable.hpp"
#include "ltb/window/window.hpp"

// generated
#include "ltb/ltb_config.hpp"

namespace ltb::app
{

class CfdLesson1App : public App
{
public:
    CfdLesson1App( )           = default;
    ~CfdLesson1App( ) override = default;

    auto initialize( glm::ivec2 framebuffer_size ) -> utils::Result< void > override;
    auto render( ) -> void override;
    auto configure_gui( ) -> void override;
    auto destroy( ) -> void override;

    auto resize( glm::ivec2 framebuffer_size ) -> void override;

private:
    std::chrono::steady_clock::time_point start_time_ = { };

    // Framebuffers and textures to store the wave field.
    static constexpr auto framebuffer_count_ = 3_UZ;

    cfd::CfdOptions< 1 > cfd_options_ = { };

    glm::ivec2                                  framebuffer_size_ = { };
    ogl::FramebufferChain< framebuffer_count_ > wave_field_chain_ = { };

    gui::IncrementalIdGenerator< uint32 > incremental_id_generator_ = { };

    struct PropagatePipeline
    {
        ogl::Shader< GL_VERTEX_SHADER > vertex_shader = {
            config::shader_dir_path( ) / "fullscreen.vert",
        };
        ogl::Shader< GL_FRAGMENT_SHADER > fragment_shader = {
            config::shader_dir_path( ) / "cfd" / "lesson1.frag",
        };
        ogl::Program program = { vertex_shader, fragment_shader };

        ogl::Uniform< ogl::Texture > prev_state_uniform       = { program, "prev_state" };
        ogl::Uniform< glm::vec2 >    framebuffer_size_uniform = { program, "framebuffer_size" };

        ogl::VertexArray vertex_array = { };
    };

    PropagatePipeline propagate_pipeline_ = { };

    struct WaveDisplayPipeline
    {
        ogl::Shader< GL_VERTEX_SHADER > vertex_shader = {
            config::shader_dir_path( ) / "cfd" / "wave_display.vert",
        };
        ogl::Shader< GL_FRAGMENT_SHADER > fragment_shader = {
            config::shader_dir_path( ) / "cfd" / "wave_display.frag",
        };
        ogl::Program program = { vertex_shader, fragment_shader };

        ogl::Uniform< ogl::Texture > wave_values_uniform = { program, "wave_values" };
        ogl::Uniform< int32 >        resolution_uniform  = { program, "resolution" };

        ogl::Buffer      vertex_buffer = { };
        ogl::VertexArray vertex_array  = { };
    };

    WaveDisplayPipeline wave_display_pipeline_ = { };

    gui::MeshDisplayPipeline mesh_display_pipeline = { incremental_id_generator_ };

    auto propagate_waves( ) -> void;
    auto display_waves( ) -> void;
};

} // namespace ltb::app
