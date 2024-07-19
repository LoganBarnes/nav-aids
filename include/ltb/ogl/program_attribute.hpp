#pragma once

// project
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/program.hpp"

namespace ltb::ogl
{

template < typename ValueType >
class Attribute
{
public:
    using Type = ValueType;

    // NOLINTNEXTLINE(*-explicit-constructor)
    explicit( false ) Attribute( Program& program );

    auto initialize( std::string_view name ) -> utils::Result<>;

    [[nodiscard( "Const getter" )]]
    auto location( ) const -> GLuint;

private:
    Program& program_;
    GLuint   location_ = GL_INVALID_INDEX;
};

template < typename ValueType >
Attribute< ValueType >::Attribute( Program& program )
    : program_( program )
{
}

template < typename ValueType >
auto Attribute< ValueType >::initialize( std::string_view name ) -> utils::Result<>
{
    auto const location = glGetAttribLocation( program_.data( ).gl_id, name.data( ) );
    if ( location < 0 )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Cannot find attribute location for '{}'", name );
    }
    location_ = static_cast< GLuint >( location );

    return utils::success( );
}

template < typename ValueType >
auto Attribute< ValueType >::location( ) const -> GLuint
{
    return location_;
}

} // namespace ltb::ogl
