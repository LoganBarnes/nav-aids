#pragma once

// project
#include "ltb/utils/result.hpp"

// external
#include <glm/glm.hpp>

// standard
#include <optional>

namespace ltb::window
{

struct WindowSettings
{
    /// \brief The text displayed on the window title bar.
    std::string title = "Window";

    /// \brief The initial size of the window. Fullscreen if nullopt.
    std::optional< glm::ivec2 > initial_size = std::nullopt;
};

class Window
{
public:
    virtual ~Window( ) = 0;

    /// \brief Initializes a window and returns the initial size of the framebuffer.
    virtual auto initialize( WindowSettings settings ) -> utils::Result< glm::ivec2 > = 0;

    /// \brief Polls events for the window.
    virtual auto poll_events( ) -> void = 0;

    /// \brief Displays the background rendered frame on the screen.
    virtual auto swap_buffers( ) -> void = 0;

    /// \brief Returns true if a window close event was requested.
    [[nodiscard( "Const getter" )]]
    virtual auto should_close( ) const -> bool = 0;

    /// \brief Returns the new size of the framebuffer if the window was resized.
    [[nodiscard( "Const getter" )]]
    virtual auto resized( ) const -> std::optional< glm::ivec2 > = 0;
};

inline Window::~Window( ) = default;

} // namespace ltb::window
