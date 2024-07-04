#pragma once

// project
#include "ltb/window/window.hpp"

namespace ltb::window
{

class GlfwWindow : public Window
{
public:
    explicit GlfwWindow( );
    ~GlfwWindow( ) override;

    auto initialize( WindowSettings settings ) -> utils::Result< glm::ivec2 > override;
    auto poll_events( ) -> void override;
    auto swap_buffers( ) -> void override;

    [[nodiscard( "Const getter" )]]
    auto should_close( ) const -> bool override;

    [[nodiscard( "Const getter" )]]
    auto resized( ) const -> std::optional< glm::ivec2 > override;

    // The data is forward declared and defined in the .cpp file.
    // This prevents implementation details from leaking into the header file.
    struct Data;

private:
    std::unique_ptr< Data > data_;
};

} // namespace ltb::window
