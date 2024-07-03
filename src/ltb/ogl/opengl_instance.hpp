// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/ogl/opengl.hpp"
#include "ltb/utils/result.hpp"

namespace ltb::ogl
{

class OpenglInstance
{
public:
    /// \brief Creates an uninitialized instance.
    OpenglInstance( ) = default;

    /// \brief Returns true if the class has been initialized.
    explicit operator bool( ) const;

    class Builder;

private:
    explicit OpenglInstance( int opengl );

    int opengl_ = { };
};

using DebugCallback = void ( * )(
    GLenum        source,
    GLenum        type,
    GLuint        id,
    GLenum        severity,
    GLsizei       length,
    GLchar const* message,
    void const*   user_param
);

class OpenglInstance::Builder
{
public:
    Builder( ) = default;

    auto set_debug_callback( DebugCallback callback ) -> Builder&;
    auto use_default_debug_callback( ) -> Builder&;

    [[nodiscard]] auto build( ) const -> utils::Result< OpenglInstance >;

private:
    DebugCallback debug_callback_ = nullptr;
};

} // namespace ltb::ogl
