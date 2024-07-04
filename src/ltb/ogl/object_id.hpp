#pragma once

// graphics
#include "opengl.hpp"

// project
#include "ltb/ogl/fwd.hpp"
#include "ltb/utils/result.hpp"

// standard
#include <memory>

namespace ltb::ogl
{

template < typename DeleteFunc >
struct Deleter
{
    GLuint     id;
    DeleteFunc delete_func;

    template < typename Ignored >
    auto operator( )( Ignored const* const unused ) const
    {
        utils::ignore( unused );
        delete_func( 1, &id );
    }
};

template < typename GenFunc, typename DeleteFunc >
auto initialize( GLuint& id, GenFunc gen_func, DeleteFunc delete_func )
    -> utils::Result< std::shared_ptr< void > >
{
    gen_func( 1, &id );
    if ( 0u == id )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Failed to generate OpenGL ID" );
    }

    return std::shared_ptr< void >( &id, Deleter{ id, delete_func } );
}

} // namespace ltb::ogl
