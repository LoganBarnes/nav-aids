#pragma once

// project
#include "ltb/window/window.hpp"

// external
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ltb::window
{

class GlfwWindow : public Window
{
public:
    ~GlfwWindow( ) override = default;

    auto initialize( WindowSettings const& settings ) -> utils::Result< glm::ivec2 > override;
    auto poll_events( ) -> void override;
    auto swap_buffers( ) -> void override;

    [[nodiscard( "Const getter" )]]
    auto should_close( ) const -> bool override;

    [[nodiscard( "Const getter" )]]
    auto resized( ) const -> std::optional< glm::ivec2 > override;

    [[nodiscard( "Const getter" )]]
    auto glfw_window( ) -> GLFWwindow*;

private:
    std::shared_ptr< int32_t const > glfw_   = nullptr;
    std::shared_ptr< GLFWwindow >    window_ = nullptr;

    std::optional< glm::ivec2 > resized_framebuffer_ = std::nullopt;

    static auto key_quit_callback(
        GLFWwindow* window,
        int32_t     key,
        int32_t     scancode,
        int32_t     action,
        int32_t     mods
    ) -> void;

    static auto resize_callback( GLFWwindow* window, int32_t width, int32_t height ) -> void;
};

} // namespace ltb::window
