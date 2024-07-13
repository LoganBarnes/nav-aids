#pragma once

// project
#include "ltb/gui/imgui_setup.hpp"
#include "ltb/ogl/opengl_loader.hpp"
#include "ltb/utils/result.hpp"
#include "ltb/window/window.hpp"

// external
#include <glm/glm.hpp>

// standard
#include <memory>

namespace ltb::app
{

class App
{
public:
    virtual ~App( ) = 0;

    virtual auto initialize( glm::ivec2 framebuffer_size ) -> utils::Result< void > = 0;
    virtual auto render( ) -> void                                                  = 0;
    virtual auto configure_gui( ) -> void                                           = 0;
    virtual auto destroy( ) -> void                                                 = 0;
    virtual auto resize( glm::ivec2 framebuffer_size ) -> void                      = 0;
};

inline App::~App( ) = default;

/// \brief The main application window for the VOR app.
class AppRunner
{
public:
    explicit AppRunner(
        window::Window&                                                 window,
        gui::ImguiSetup&                                                imgui_setup,
        std::vector< std::pair< std::string, std::shared_ptr< App > > > apps
    );

    auto initialize( ) -> utils::Result< AppRunner* >;
    auto run( ) -> utils::Result< void >;

private:
    // Window & graphics interfaces
    window::Window&  window_;
    gui::ImguiSetup& imgui_setup_;

    glm::ivec2        framebuffer_size_ = { };
    ogl::OpenglLoader ogl_loader_       = { };

    std::vector< std::pair< std::string, std::shared_ptr< App > > > apps_;
    App*                                                            current_app_ = nullptr;

    auto render_gui( ) -> void;
    auto configure_app_menu( std::string const& name, App* app ) -> void;
};

} // namespace ltb::app
