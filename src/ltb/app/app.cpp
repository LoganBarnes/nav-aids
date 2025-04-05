#include "ltb/app/app.hpp"

// project
#include "ltb/ogl/utils.hpp"

// external
#include <spdlog/spdlog.h>

// standard
#include <algorithm>
#include <ranges>

namespace ltb::app
{
namespace
{

[[maybe_unused]] auto opengl_error_callback(
    GLenum        source,
    GLenum        type,
    GLuint        id,
    GLenum        severity,
    GLsizei       length,
    GLchar const* message,
    void const*   userParam
)
{
    utils::ignore( source, type, id, length, userParam );
    spdlog::error( "OpenGL Error: {} ({}: {})", message, id, severity );
}

} // namespace

AppRunner::AppRunner(
    window::Window&                                                 window,
    gui::ImguiSetup&                                                imgui_setup,
    std::vector< std::pair< std::string, std::shared_ptr< App > > > apps
)
    : window_( window )
    , imgui_setup_( imgui_setup )
    , apps_( std::move( apps ) )
{
}

auto AppRunner::initialize( ) -> utils::Result< AppRunner* >
{
    LTB_CHECK(
        framebuffer_size_,
        window_.initialize( {
            .title        = "Navigational Aids",
            .initial_size = glm::ivec2{ 1280, 720 },
        } )
    );
    LTB_CHECK( imgui_setup_.initialize( ) );

    ImGui::GetIO( ).ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // LTB_CHECK( ogl_loader_.initialize( ) );
    LTB_CHECK( ogl_loader_.initialize( opengl_error_callback ) );

    ogl::set_defaults( );

    if ( !apps_.empty( ) )
    {
        current_app_ = apps_.front( ).second.get( );
        LTB_CHECK( current_app_->initialize( framebuffer_size_ ) );
    }

    return this;
}

auto AppRunner::run( ) -> utils::Result< void >
{
    while ( !window_.should_close( ) )
    {
        window_.poll_events( );

        if ( auto const framebuffer_size = window_.resized( ) )
        {
            framebuffer_size_ = framebuffer_size.value( );
            spdlog::info( "Resized window to: {}x{}", framebuffer_size_.x, framebuffer_size_.y );
            if ( nullptr != current_app_ )
            {
                current_app_->resize( framebuffer_size_ );
            }
        }

        render( );

        window_.swap_buffers( );
    }

    return utils::success( );
}

auto AppRunner::render( ) -> void
{
    imgui_setup_.new_frame( );

    if ( ImGui::BeginMainMenuBar( ) )
    {
        if ( ImGui::BeginMenu( "Apps" ) )
        {
            for ( auto const& [ name, app ] : apps_ )
            {
                configure_app_menu( name, app.get( ) );
            }
            ImGui::EndMenu( );
        }

        ImGui::Text( "(%.1f FPS)", ImGui::GetIO( ).Framerate );

        ImGui::EndMainMenuBar( );
    }

    if ( nullptr != current_app_ )
    {
        current_app_->configure_gui( );
        current_app_->render( );
    }

    imgui_setup_.render( );
}

auto AppRunner::configure_app_menu( std::string const& name, App* const app ) -> void
{
    if ( ImGui::MenuItem( name.c_str( ) ) )
    {
        spdlog::info( "Switching to app: {}", name );

        if ( nullptr != current_app_ )
        {
            current_app_->destroy( );
        }

        current_app_ = app;

        if ( auto const result = current_app_->initialize( framebuffer_size_ ); !result )
        {
            spdlog::error( "Failed to initialize app: {}", result.error( ).error_message( ) );
            current_app_ = nullptr;
        }
    }
}

} // namespace ltb::app
