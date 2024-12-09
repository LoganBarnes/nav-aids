#include "ltb/app/cfd_lesson_1_app.hpp"

// project
#include "ltb/ogl/program_attribute.hpp"
#include "ltb/utils/error_callback.hpp"
#include "ltb/utils/initializable.hpp"

namespace ltb::app
{
namespace
{

//auto constexpr draw_start_vertex       = 0;
//auto constexpr fullscreen_draw_mode    = GL_TRIANGLE_STRIP;
//auto constexpr fullscreen_vertex_count = 4;

} // namespace

auto CfdLesson1App::initialize( glm::ivec2 const framebuffer_size ) -> utils::Result< void >
{
    framebuffer_size_ = framebuffer_size;

    LTB_CHECK( wave_field_chain_.initialize( framebuffer_size_ ) );

    LTB_CHECK(
        utils::initialize(
            propagate_pipeline_.vertex_shader,
            propagate_pipeline_.fragment_shader,
            propagate_pipeline_.program,

            propagate_pipeline_.wave_texture_uniform,
            propagate_pipeline_.framebuffer_size_uniform,

            propagate_pipeline_.vertex_buffer,
            propagate_pipeline_.vertex_array
        )
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
    propagate_pipeline_ = { };
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

    glViewport( 0, 0, cfd_options_.domain_resolution, 1 );
    glClear( GL_COLOR_BUFFER_BIT );
}

auto CfdLesson1App::display_waves( ) -> void
{
    glViewport( 0, 0, framebuffer_size_.x, framebuffer_size_.y );
    glClear( GL_COLOR_BUFFER_BIT );
}

} // namespace ltb::app
