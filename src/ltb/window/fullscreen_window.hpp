#pragma once

// project
#include "ltb/utils/result.hpp"

// external
#include <glm/glm.hpp>

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
    virtual auto should_close( ) const -> bool = 0;
};

inline FullscreenWindow::~FullscreenWindow( ) = default;

} // namespace ltb::window
