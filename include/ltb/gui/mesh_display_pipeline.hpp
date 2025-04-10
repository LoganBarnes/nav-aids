#pragma once

// project
#include "ltb/gui/incremental_id_generator.hpp"
#include "ltb/gui/mesh_display_settings.hpp"
#include "ltb/gui/mesh_id.hpp"
#include "ltb/math/cam/camera_render_params.hpp"
#include "ltb/math/mesh.hpp"
#include "ltb/ogl/program_attribute.hpp"
#include "ltb/ogl/program_uniform.hpp"
#include "ltb/ogl/vertex_array.hpp"

// generated
#include "ltb/ltb_config.hpp"

namespace ltb::gui
{

using MeshId = TypeId< struct MeshIdTag >;

class MeshDisplayPipeline
{
public:
    explicit( false ) MeshDisplayPipeline( IncrementalIdGenerator< uint32 >& id_generator );

    /// \brief Initialize all the pipeline graphics objects.
    /// \note This should be called before any other functions to avoid errors.
    auto initialize( ) -> utils::Result< void >;

    /// \brief Returns true if the pipeline has been initialized.
    [[nodiscard( "Const getter" )]]
    auto is_initialized( ) const -> bool;

    /// \brief Create new GPU objects used to display the mesh data.
    auto initialize_mesh( math::Mesh3 const& mesh ) -> utils::Result< MeshId >;

    /// \brief Update the display settings for the mesh with the provided id. If errors are
    ///        encountered they are logged to the console and the settings are not changed.
    auto update_settings( MeshId id, MeshDisplaySettings settings ) -> void;

    /// \brief Returns the saved display settings for the mesh with the provided id.
    ///        If errors are encountered they are logged to the console and the default
    ///        settings are returned.
    [[nodiscard( "Const getter" )]]
    auto get_settings( MeshId id ) const -> MeshDisplaySettings;

    /// \brief Remove the mesh with the provided id from the pipeline. If errors are
    ///        encountered they are logged to the console and the mesh is not changed.
    auto remove( MeshId id ) -> void;

    /// \brief Remove all meshes from the pipeline.
    auto clear( ) -> void;

    /// \brief Render the meshes in 3D using the provided camera parameters.
    ///        If errors are encountered they are logged to the console and nothing is rendered.
    auto draw( math::cam::CameraRenderParams const& cam ) -> void;

private:
    IncrementalIdGenerator< uint32 >& id_generator_;

    // OpenGL GPU shader program
    ogl::Shader< GL_VERTEX_SHADER > vertex_shader_ = {
        config::shader_dir_path( ) / "mesh.vert",
    };
    ogl::Shader< GL_FRAGMENT_SHADER > fragment_shader_ = {
        config::shader_dir_path( ) / "mesh.frag",
    };
    ogl::Program program_ = {
        vertex_shader_,
        fragment_shader_,
    };

    // Program attributes passed to the GPU by vertex buffers
    ogl::Attribute< glm::vec3 > local_position_attribute_ = {
        program_,
        "local_position",
    };
    ogl::Attribute< glm::vec3 > local_normal_attribute_ = {
        program_,
        "local_normal",
    };
    ogl::Attribute< glm::vec2 > local_uv_coords_attribute_ = {
        program_,
        "local_uv_coords",
    };
    ogl::Attribute< glm::vec3 > local_color_attribute_ = {
        program_,
        "local_color",
    };

    // Program uniforms passed to the GPU during render calls
    ogl::Uniform< glm::mat4 > clip_from_world_uniform_ = {
        program_,
        "clip_from_world",
    };
    ogl::Uniform< glm::mat4 > world_from_local_uniform_ = {
        program_,
        "world_from_local",
    };
    ogl::Uniform< glm::mat3 > world_from_local_normals_uniform_ = {
        program_,
        "world_from_local_normals",
    };
    ogl::Uniform< glm::vec4 > uniform_color_uniform_ = {
        program_,
        "uniform_color",
    };
    ogl::Uniform< uint32 > color_mode_uniform_ = {
        program_,
        "color_mode",
    };
    ogl::Uniform< uint32 > shading_mode_uniform_ = {
        program_,
        "shading_mode",
    };

    // The initialization state of the pipeline.
    bool initialized_ = false;

    struct InternalMeshData
    {
        std::vector< ogl::Buffer >   vertex_buffers = { };
        ogl::VertexArray             vertex_array   = { };
        std::optional< ogl::Buffer > index_buffer   = std::nullopt;

        GLenum  draw_mode  = GL_POINTS;
        GLsizei draw_count = 0;

        MeshDisplaySettings settings = { };

        // The world transform applied to this mesh.
        glm::mat4 world_transform = glm::identity< glm::mat4 >( );

        // The world transform applied to mesh normals.
        glm::mat3 normals_transform = glm::identity< glm::mat3 >( );
    };

    // All the mesh buffers and their associated data.
    std::unordered_map< MeshId, InternalMeshData > internal_data_ = { };

    // A helper struct used to check if a buffer should be displayed
    // based on its current settings.
    struct ShouldDisplay;
};

} // namespace ltb::gui
