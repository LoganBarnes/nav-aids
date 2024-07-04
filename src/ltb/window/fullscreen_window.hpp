#pragma once

// project
#include "ltb/utils/result.hpp"

// external
#include <glm/glm.hpp>

// standard
#include <optional>

namespace ltb::window
{

class FullscreenWindow
{
public:
    virtual ~FullscreenWindow( ) = 0;

    /// \brief Initializes a window and returns the initial size of the framebuffer.
    virtual auto initialize( std::string_view window_title ) -> utils::Result< glm::ivec2 > = 0;

    /// \brief Polls events for the window.
    virtual auto poll_events( ) -> void = 0;

    /// \brief Displays the background rendered frame on the screen.
    virtual auto swap_buffers( ) -> void = 0;

    /// \brief Returns true if a window close event was requested.
    [[nodiscard( "Const getter" )]]
    virtual auto should_close( ) const -> bool = 0;

    /// \brief Returns the new size of the framebuffer if the window was resized.
    [[nodiscard("Const getter")]]
    virtual auto resized( ) const -> std::optional< glm::ivec2 > = 0;
};

inline FullscreenWindow::~FullscreenWindow( ) = default;

} // namespace ltb::window
