#pragma once

// graphics
#include "ltb/ogl/opengl.hpp"

namespace ltb::ogl::objs
{

class MeshPipeline
{
public:
    MeshPipeline( )          = default;
    virtual ~MeshPipeline( ) = default;

    /// \brief Initialize the framebuffer object. This must
    ///        be called before using the framebuffer.
    auto initialize( ) -> void;

    /// \brief Returns whether the framebuffer has been successfully initialized.
    [[nodiscard( "Const getter" )]]
    auto is_initialized( ) const -> bool;
};

} // namespace ltb::ogl::objs
