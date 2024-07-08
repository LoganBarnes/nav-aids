#include "ltb/gui/glfw_opengl_imgui_setup.hpp"

// external
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>

namespace ltb::gui
{
namespace
{

struct ImGuiGlfwDeleter
{
    auto operator( )( auto* p ) const -> void
    {
        spdlog::debug( "ImGui_ImplGlfw_Shutdown" );
        ImGui_ImplGlfw_Shutdown( );
        delete p;
    }
};

struct ImGuiOpenGLDeleter
{
    auto operator( )( auto* p ) const -> void
    {
        spdlog::debug( "ImGui_ImplOpenGL3_Shutdown" );
        ImGui_ImplOpenGL3_Shutdown( );
        delete p;
    }
};

} // namespace

GlfwOpenglImguiSetup::GlfwOpenglImguiSetup( window::GlfwWindow& window )
    : window_{ window }
{
}

auto GlfwOpenglImguiSetup::initialize( ) -> utils::Result<>
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION( );

    auto context = std::unique_ptr< ImGuiContext, decltype( &ImGui::DestroyContext ) >(
        // Context will be deleted if this function fails.
        ImGui::CreateContext( ),
        &ImGui::DestroyContext
    );
    if ( nullptr == context )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "ImGui::CreateContext failed." );
    }
    spdlog::debug( "ImGui::CreateContext()" );

    auto imgui_glfw = std::unique_ptr< bool, ImGuiGlfwDeleter >(
        new bool( ImGui_ImplGlfw_InitForOpenGL( window_.glfw_window( ), true ) )
    );
    if ( !*imgui_glfw )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "ImGui_ImplGlfw_InitForOpenGL failed." );
    }
    spdlog::debug( "ImGui_ImplGlfw_InitForOpenGL()" );

    auto imgui_opengl
        = std::unique_ptr< bool, ImGuiOpenGLDeleter >( new bool( ImGui_ImplOpenGL3_Init( ) ) );
    if ( !*imgui_opengl )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "ImGui_ImplOpenGL3_Init failed." );
    }
    spdlog::debug( "ImGui_ImplOpenGL3_Init succeeded." );

    context_      = std::move( context );
    imgui_glfw_   = std::move( imgui_glfw );
    imgui_opengl_ = std::move( imgui_opengl );

    return utils::success( );
}

auto GlfwOpenglImguiSetup::new_frame( ) const -> void
{
    ImGui_ImplOpenGL3_NewFrame( );
    ImGui_ImplGlfw_NewFrame( );
    ImGui::NewFrame( );
}

auto GlfwOpenglImguiSetup::render( ) const -> void
{
    ImGui::Render( );
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData( ) );
}

} // namespace ltb::gui
