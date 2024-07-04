// /////////////////////////////////////////////////////////////
// Copyright (c) Rotor Technologies, Inc. - All Rights Reserved
// /////////////////////////////////////////////////////////////
#include "ltb/window/fullscreen_glfw_window.hpp"

// Graphics
#include "ltb/ogl/opengl.hpp"

// project
#include "ltb/utils/ignore.hpp"

// external
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

namespace ltb::window
{
namespace
{

// This is the highest OpenGL version macOS will support.
[[maybe_unused]] constexpr auto opengl_major_version = 4;
[[maybe_unused]] constexpr auto opengl_minor_version = 0;

auto default_glfw_error_callback( int32_t const error, char const* const description ) -> void
{
    spdlog::error( "GLFW Error ({}): {}", error, description );
}

struct GlfwDeleter
{
    auto operator( )( int32_t const* const unused ) const -> void
    {
        // This pointer has already been deleted, so it should be ignored.
        utils::ignore( unused );
        ::glfwTerminate( );
    }
};

struct GlfwWindowDeleter
{
    auto operator( )( GLFWwindow* const window ) const -> void { ::glfwDestroyWindow( window ); }
};

auto key_quit_callback(
    GLFWwindow* const window,
    int32_t const     key,
    int32_t const     scancode,
    int32_t const     action,
    int32_t const     mods
) -> void
{
    utils::ignore( scancode );

    auto const shift_down = 0 != ( mods & GLFW_MOD_SHIFT );
    auto const ctrl_down  = 0 != ( mods & GLFW_MOD_CONTROL );
    auto const q_released = ( GLFW_KEY_Q == key ) && ( GLFW_RELEASE == action );

    if ( q_released && ctrl_down && shift_down )
    {
        ::glfwSetWindowShouldClose( window, GLFW_TRUE );
    }
}

} // namespace

struct FullscreenGlfwWindow::Data
{
    std::unique_ptr< int32_t const, GlfwDeleter >    glfw   = nullptr;
    std::unique_ptr< GLFWwindow, GlfwWindowDeleter > window = nullptr;
};

// This constructor and destructor must be defined in the cpp
// file since the unique_ptr<Data> requires the full definition
// of Data when it is instantiated and destroyed.
FullscreenGlfwWindow::FullscreenGlfwWindow( )  = default;
FullscreenGlfwWindow::~FullscreenGlfwWindow( ) = default;

auto FullscreenGlfwWindow::initialize( std::string_view const window_title
) -> utils::Result< glm::ivec2 >
{
    data_ = std::make_unique< Data >( );

    // Set the error callback before any GLFW calls to log when things go wrong.
    // The previous callback is ignored because it doesn't need to be restored.
    ltb::utils::ignore( ::glfwSetErrorCallback( default_glfw_error_callback ) );

    // Initialize the window framework library.
    auto const maybe_glfw = ::glfwInit( );
    if ( GLFW_FALSE == maybe_glfw )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "glfwInit() failed" );
    }
    // This pointer to a local variable will be invalid when GlfwDeleter is called, but
    // GlfwDeleter does not do anything with the pointer, so it will not cause a problem.
    data_->glfw = std::unique_ptr< int32_t const, GlfwDeleter >{ &maybe_glfw };

    ::glfwWindowHint( GLFW_VISIBLE, GLFW_TRUE );
    ::glfwWindowHint( GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE );

    // Request the preferred OpenGL Profile.
    ::glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    ::glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
    ::glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, opengl_major_version );
    ::glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, opengl_minor_version );
    ::glfwWindowHint( GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API );

    // Get the current monitor settings to create a fullscreen window with the same settings.
    auto* const monitor = ::glfwGetPrimaryMonitor( );
    if ( nullptr == monitor )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Monitor not found" );
    }

    auto const* const video_mode = ::glfwGetVideoMode( monitor );
    if ( nullptr == video_mode )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "glfwGetVideoMode() failed" );
    }
    ::glfwWindowHint( GLFW_RED_BITS, video_mode->redBits );
    ::glfwWindowHint( GLFW_GREEN_BITS, video_mode->greenBits );
    ::glfwWindowHint( GLFW_BLUE_BITS, video_mode->blueBits );
    ::glfwWindowHint( GLFW_REFRESH_RATE, video_mode->refreshRate );

    // Get the work area of the monitor to create a window that fits
    // the screen without overlapping the taskbar or other system UI.
    struct WorkArea
    {
        glm::ivec2 position = { };
        glm::ivec2 size     = { };
    };

    auto work_area = WorkArea{ };

    ::glfwGetMonitorWorkarea(
        monitor,
        &work_area.position.x,
        &work_area.position.y,
        &work_area.size.x,
        &work_area.size.y
    );

    // The window does not need to be resized since it will
    // already be the appropriate size for the current monitor.
    ::glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

    // The title bar pushes the window down below the work area, so it is
    // disabled. The window can still be closed by using the taskbar icon.
    ::glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );

    auto* const maybe_window = ::glfwCreateWindow(
        work_area.size.x,
        work_area.size.y,
        window_title.data( ),
        nullptr,
        nullptr
    );
    if ( nullptr == maybe_window )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "glfwCreateWindow() failed" );
    }
    data_->window = std::unique_ptr< GLFWwindow, GlfwWindowDeleter >{ maybe_window };

    // Set the current context
    ::glfwMakeContextCurrent( data_->window.get( ) );
    ::glfwSwapInterval( 1 );

    // Ignore the old callback.
    utils::ignore( ::glfwSetKeyCallback( data_->window.get( ), &key_quit_callback ) );

    // Get and return the current framebuffer size for any graphics APIs using the window.
    auto framebuffer_size = glm::ivec2{ };
    ::glfwGetFramebufferSize( data_->window.get( ), &framebuffer_size.x, &framebuffer_size.y );

    return framebuffer_size;
}

auto FullscreenGlfwWindow::poll_events( ) -> void
{
    ::glfwPollEvents( );
}

auto FullscreenGlfwWindow::swap_buffers( ) -> void
{
    ::glfwSwapBuffers( data_->window.get( ) );
}

auto FullscreenGlfwWindow::should_close( ) const -> bool
{
    return ::glfwWindowShouldClose( data_->window.get( ) );
}

} // namespace ltb::window
