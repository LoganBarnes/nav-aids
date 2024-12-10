#pragma once

// project
#include "ltb/gui/incremental_id_generator.hpp"
#include "ltb/math/cam/camera_render_params.hpp"
#include "ltb/math/transforms.hpp"
#include "ltb/ogl/program_attribute.hpp"
#include "ltb/ogl/program_uniform.hpp"
#include "ltb/ogl/vertex_array.hpp"

// generated
#include "ltb/ltb_config.hpp"

namespace ltb::cfd::gui
{

template < glm::length_t Dimensions >
class WaveDisplayPipeline;

template <>
class WaveDisplayPipeline< math::one_dimension >
{
public:
    explicit( false
    ) WaveDisplayPipeline( ltb::gui::IncrementalIdGenerator< uint32 >& id_generator );

    /// \brief Initialize all the pipeline graphics objects.
    /// \note This should be called before any other functions to avoid errors.
    auto initialize( ) -> utils::Result< void >;

    /// \brief Returns true if the pipeline has been initialized.
    [[nodiscard( "Const getter" )]]
    auto is_initialized( ) const -> bool;

    /// \brief Render the meshes in 3D using the provided camera parameters.
    ///        If errors are encountered they are logged to the console and nothing is rendered.
    auto draw(
        math::cam::CameraRenderParams const& cam,
        ogl::Texture const&                  wave_values,
        int32                                texture_resolution
    ) -> void;

    auto resize( glm::ivec2 framebuffer_size ) -> void;

private:
    ogl::Shader< GL_VERTEX_SHADER > vertex_shader_ = {
        config::shader_dir_path( ) / "cfd" / "wave_display.vert",
    };
    ogl::Shader< GL_FRAGMENT_SHADER > fragment_shader_ = {
        config::shader_dir_path( ) / "cfd" / "wave_display.frag",
    };
    ogl::Program program_ = {
        vertex_shader_,
        fragment_shader_,
    };

    ogl::Uniform< ogl::Texture > wave_values_uniform_ = {
        program_,
        "wave_values",
    };
    ogl::Uniform< int32 > resolution_uniform_ = {
        program_,
        "resolution",
    };

    ogl::Buffer      vertex_buffer_ = { };
    ogl::VertexArray vertex_array_  = { };
};

} // namespace ltb::cfd::gui
