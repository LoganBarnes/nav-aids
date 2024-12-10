#include "ltb/app/cfd_lesson_1_app.hpp"

// project
#include "ltb/ogl/program_attribute.hpp"
#include "ltb/utils/error_callback.hpp"
#include "ltb/utils/initializable.hpp"

// external
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>

namespace ltb::app
{
namespace
{

auto constexpr draw_start_vertex       = 0;
auto constexpr fullscreen_draw_mode    = GL_TRIANGLE_STRIP;
auto constexpr fullscreen_vertex_count = 4;

struct IndexToVec2
{
    auto operator( )( int32 const i ) { return glm::ivec2( i / 2, i % 2 ); }
};

} // namespace

auto CfdLesson1App::initialize( glm::ivec2 const framebuffer_size ) -> utils::Result< void >
{
    framebuffer_size_ = framebuffer_size;

    LTB_CHECK( wave_field_chain_.initialize( glm::ivec2( cfd::resolution_extents.max, 1 ) ) );

    LTB_CHECK(
        utils::initialize(
            propagate_pipeline_.vertex_shader,
            propagate_pipeline_.fragment_shader,
            propagate_pipeline_.program,

            propagate_pipeline_.prev_state_uniform,
            propagate_pipeline_.framebuffer_size_uniform,

            propagate_pipeline_.vertex_array
        )
    );

    LTB_CHECK(
        utils::initialize(
            wave_display_pipeline_.vertex_shader,
            wave_display_pipeline_.fragment_shader,
            wave_display_pipeline_.program,

            wave_display_pipeline_.wave_values_uniform,
            wave_display_pipeline_.resolution_uniform,

            wave_display_pipeline_.vertex_buffer,
            wave_display_pipeline_.vertex_array
        )
    );

    auto const indices = ranges::view::iota( 0, cfd::resolution_extents.max * 2 )
                       | ranges::view::transform( IndexToVec2{ } ) | ranges::to< std::vector >( );

    auto const bound_buffer = bind< GL_ARRAY_BUFFER >( wave_display_pipeline_.vertex_buffer );
    buffer_data( bound_buffer, indices, GL_STATIC_DRAW );

    auto const index_attribute = ogl::Attribute< glm::ivec2 >{
        wave_display_pipeline_.program,
        "index",
    };

    // Tightly packed.
    constexpr auto total_vertex_stride = 0U;
    // Not instanced
    constexpr auto attrib_divisor = 0U;
    set_attributes(
        bind( wave_display_pipeline_.vertex_array ),
        bound_buffer,
        {
            {
                .attribute_location      = index_attribute.location( ),
                .num_coordinates         = glm::vec2::length( ),
                .data_type               = GL_FLOAT,
                .initial_offset_into_vbo = nullptr,
            },
        },
        total_vertex_stride,
        attrib_divisor
    );

    glClearColor( 0.0F, 0.0F, 0.0F, 1.0F );
    glDisable( GL_DEPTH_TEST );

    start_time_ = std::chrono::steady_clock::now( );
    return utils::success( );
}

auto CfdLesson1App::render( ) -> void
{
    propagate_waves( );
    display_waves( );
}

auto CfdLesson1App::configure_gui( ) -> void
{
    auto const dock_node_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    utils::ignore( ImGui::DockSpaceOverViewport( 0, nullptr, dock_node_flags ) );

    if ( ImGui::Begin( "CFD Lesson 1" ) )
    {
        cfd::configure_gui( cfd_options_ );
    }
    ImGui::End( );
}

auto CfdLesson1App::destroy( ) -> void
{
    wave_display_pipeline_ = { };
    propagate_pipeline_    = { };
}

auto CfdLesson1App::resize( glm::ivec2 const framebuffer_size ) -> void
{
    framebuffer_size_ = framebuffer_size;
    LTB_CHECK_OR( wave_field_chain_.resize( framebuffer_size_ ), utils::log_error );
}

auto CfdLesson1App::propagate_waves( ) -> void
{
    wave_field_chain_.swap( );

    auto const bound_framebuffer
        = ogl::bind< GL_FRAMEBUFFER >( wave_field_chain_.get_framebuffer< 0 >( ) );

    auto const framebuffer_size = glm::ivec2( cfd_options_.domain_resolution, 1 );

    glViewport( 0, 0, framebuffer_size.x, framebuffer_size.y );
    glClear( GL_COLOR_BUFFER_BIT );

    ogl::set( propagate_pipeline_.framebuffer_size_uniform, glm::vec2( framebuffer_size ) );

    auto const& previous_state = wave_field_chain_.get_texture< 1 >( );

    auto const active_tex_0 = GLint{ 0 };
    previous_state.active_tex( active_tex_0 );

    auto const bound_prev_texture = bind< GL_TEXTURE_2D >( previous_state );
    ogl::set( propagate_pipeline_.prev_state_uniform, bound_prev_texture, active_tex_0 );

    ogl::draw(
        ogl::bind( propagate_pipeline_.program ),
        ogl::bind( propagate_pipeline_.vertex_array ),
        fullscreen_draw_mode,
        draw_start_vertex,
        fullscreen_vertex_count
    );
}

auto CfdLesson1App::display_waves( ) -> void
{
    glViewport( 0, 0, framebuffer_size_.x, framebuffer_size_.y );
    glClear( GL_COLOR_BUFFER_BIT );
}

} // namespace ltb::app
