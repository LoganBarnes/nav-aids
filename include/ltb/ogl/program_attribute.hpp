#pragma once

// project
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/program.hpp"

// external
#include <spdlog/spdlog.h>

namespace ltb::ogl
{

template < typename ValueType >
class Attribute
{
public:
    using Type = ValueType;

    Attribute( Program& program, std::string name );

    auto initialize( ) -> utils::Result<>;

    [[nodiscard( "Const getter" )]]
    auto location( ) const -> GLuint;

private:
    Program&    program_;
    std::string name_;
    GLuint      location_ = GL_INVALID_INDEX;
};

template < typename ValueType >
Attribute< ValueType >::Attribute( Program& program, std::string name )
    : program_( program )
    , name_( std::move( name ) )
{
}

template < typename ValueType >
auto Attribute< ValueType >::initialize( ) -> utils::Result<>
{
    auto const location = glGetAttribLocation( program_.data( ).gl_id, name_.c_str( ) );
    if ( location < 0 )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Cannot find attribute location for '{}'", name_ );
    }
    location_ = static_cast< GLuint >( location );
    spdlog::debug( "Attribute '{}' location: {}", name_, location_ );

    return utils::success( );
}

template < typename ValueType >
auto Attribute< ValueType >::location( ) const -> GLuint
{
    return location_;
}

} // namespace ltb::ogl
