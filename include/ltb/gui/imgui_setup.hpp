#pragma once

// project
#include "ltb/gui/imgui.hpp"
#include "ltb/utils/result.hpp"

// standard
#include <memory>

namespace ltb::gui
{

class ImguiSetup
{
public:
    virtual ~ImguiSetup( ) = 0;

    virtual auto initialize( ) -> utils::Result<> = 0;

    virtual auto new_frame( ) const -> void = 0;
    virtual auto render( ) const -> void    = 0;
};

inline ImguiSetup::~ImguiSetup( ) = default;

} // namespace ltb::gui
