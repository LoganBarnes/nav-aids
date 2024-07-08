#pragma once

// project
#include "ltb/gui/imgui_setup.hpp"
#include "ltb/window/glfw_window.hpp"

// standard
#include <memory>

namespace ltb::gui
{

class GlfwOpenglImguiSetup : public ImguiSetup
{
public:
    explicit GlfwOpenglImguiSetup( window::GlfwWindow& window );

    auto initialize( ) -> utils::Result<> override;

    auto new_frame( ) const -> void override;
    auto render( ) const -> void override;

private:
    window::GlfwWindow& window_;

    /// \brief RAII object to handle ImGui context creation and destruction.
    std::shared_ptr< ImGuiContext > context_ = nullptr;

    /// \brief RAII object to handle ImGui GLFW setup and destruction.
    std::shared_ptr< bool > imgui_glfw_ = nullptr;

    /// \brief RAII object to handle ImGui OpenGL setup and destruction.
    std::shared_ptr< bool > imgui_opengl_ = nullptr;
};

} // namespace ltb::gui
