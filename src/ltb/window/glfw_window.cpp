// /////////////////////////////////////////////////////////////
// Copyright (c) Rotor Technologies, Inc. - All Rights Reserved
// /////////////////////////////////////////////////////////////
#include "ltb/window/glfw_window.hpp"

// Graphics
#include "ltb/ogl/opengl.hpp"

// project
#include "ltb/utils/ignore.hpp"

// external
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

// standard
#include <memory>

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

} // namespace

struct GlfwWindow::Data
{
    std::unique_ptr< int32_t const, GlfwDeleter >    glfw   = nullptr;
    std::unique_ptr< GLFWwindow, GlfwWindowDeleter > window = nullptr;

    std::optional< glm::ivec2 > resized_framebuffer = std::nullopt;
};

namespace
{

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

auto resize_callback( GLFWwindow* const window, int32_t const width, int32_t const height ) -> void
{
    auto* const data = static_cast< GlfwWindow::Data* >( ::glfwGetWindowUserPointer( window ) );

    data->resized_framebuffer = glm::ivec2{ width, height };
}

} // namespace

// This constructor and destructor must be defined in the cpp
// file since the unique_ptr<Data> requires the full definition
// of Data when it is instantiated and destroyed.
GlfwWindow::GlfwWindow( )  = default;
GlfwWindow::~GlfwWindow( ) = default;

auto GlfwWindow::initialize( WindowSettings const settings ) -> utils::Result< glm::ivec2 >
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

    auto initial_size = glm::ivec2{ video_mode->width, video_mode->height };
    if ( settings.initial_size.has_value( ) )
    {
        initial_size = settings.initial_size.value( );
    }

    auto* const maybe_window = ::glfwCreateWindow(
        initial_size.x,
        initial_size.y,
        settings.title.c_str( ),
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

    ::glfwSetWindowUserPointer( data_->window.get( ), data_.get( ) );

    // Ignore the old callback.
    utils::ignore( ::glfwSetKeyCallback( data_->window.get( ), &key_quit_callback ) );

    // Get and return the current framebuffer size for any graphics APIs using the window.
    auto framebuffer_size = glm::ivec2{ };
    ::glfwGetFramebufferSize( data_->window.get( ), &framebuffer_size.x, &framebuffer_size.y );

    utils::ignore( ::glfwSetFramebufferSizeCallback( data_->window.get( ), &resize_callback ) );

    return framebuffer_size;
}

auto GlfwWindow::poll_events( ) -> void
{
    data_->resized_framebuffer = std::nullopt;
    ::glfwPollEvents( );
}

auto GlfwWindow::swap_buffers( ) -> void
{
    ::glfwSwapBuffers( data_->window.get( ) );
}

auto GlfwWindow::should_close( ) const -> bool
{
    return ::glfwWindowShouldClose( data_->window.get( ) );
}

auto GlfwWindow::resized( ) const -> std::optional< glm::ivec2 >
{
    return data_->resized_framebuffer;
}

} // namespace ltb::window