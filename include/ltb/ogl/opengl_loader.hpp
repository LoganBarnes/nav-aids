#pragma once

// project
#include "ltb/ogl/opengl.hpp"
#include "ltb/utils/result.hpp"
#include "ltb/utils/types.hpp"

namespace ltb::ogl
{

class OpenglLoader
{
public:
    /// \brief Creates an uninitialized instance.
    OpenglLoader( ) = default;

    auto initialize( ) -> utils::Result<>;
    auto initialize( GLDEBUGPROC debug_callback ) -> utils::Result<>;

    /// \brief Returns whether the loader has been successfully initialized.
    [[nodiscard( "Const getter" )]]
    auto is_initialized( ) const -> bool;

private:
    int32 opengl_ = 0;
};

} // namespace ltb::ogl
