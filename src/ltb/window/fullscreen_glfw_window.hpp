#pragma once

// project
#include "ltb/window/fullscreen_window.hpp"

namespace ltb::window
{

class FullscreenGlfwWindow : public FullscreenWindow
{
public:
    explicit FullscreenGlfwWindow( );
    ~FullscreenGlfwWindow( ) override;

    auto initialize( std::string_view window_title ) -> utils::Result< glm::ivec2 > override;
    auto poll_events( ) -> void override;
    auto swap_buffers( ) -> void override;
    auto should_close( ) const -> bool override;

private:
    // The data is forward declared and defined in the .cpp file.
    // This prevents implementation details from leaking into the header file.
    struct Data;
    std::unique_ptr< Data > data_;
};

} // namespace ltb::window
