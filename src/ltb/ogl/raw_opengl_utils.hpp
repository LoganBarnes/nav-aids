// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 Rotor Technologies, Inc - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/ogl/opengl.hpp"
#include "ltb/utils/generic_guard.hpp"

// standard
#include <array>
#include <functional>
#include <memory>

namespace ltb::ogl
{

using GenFunc    = void    ( * )( GLsizei, GLuint* );
using DeleteFunc = void ( * )( GLsizei, GLuint const* );

/// \brief Creates a shared pointer to an OpenGL resource generated
///        by `gen_func`. On deletion, `delete_func` will be called
///        to free the resource. If there are items that should exist
///        over the lifetime of this object, they can be passed to
///        this function and will be copied into the shared object.
template < typename... Deps >
auto make_shared_id( GenFunc const& gen_func, DeleteFunc delete_func, Deps&&... deps ) -> std::shared_ptr< GLuint >
{
    return std::shared_ptr< GLuint >(
        [ &gen_func ] {
            GLuint id;
            gen_func( 1, &id );
            return new GLuint( id );
        }( ),
        [ delete_func, to_save = std::make_tuple( std::forward< Deps >( deps )... ) ]( GLuint* id ) {
            delete_func( 1, id );
            delete id;
        }
    );
}

/// \brief Creates a shared pointer to an OpenGL resource generated
///        by `gen_func`. On deletion, `delete_func` will be called
///        to free the resource. If there are items that should exist
///        over the lifetime of this object, they can be passed to
///        this function and will be copied into the shared object.
template < typename T, typename... Deps >
auto make_shared_data( GenFunc const& gen_func, DeleteFunc delete_func, Deps&&... deps ) -> std::shared_ptr< T >
{
    return std::shared_ptr< T >(
        [ &gen_func ] {
            auto data = new T( );
            gen_func( 1, &data->gl_id );
            return data;
        }( ),
        [ delete_func, to_save = std::make_tuple( std::forward< Deps >( deps )... ) ]( T* data ) {
            delete_func( 1, &data->gl_id );
            delete data;
        }
    );
}

/// \brief Calls `toggle` (usually glEnable/glDisable) on the provided enum.
struct GLenumToggle
{
    GLenum setting;
    bool   should_toggle;
    void   ( *toggle )( GLenum );

    auto operator( )( ) const -> void
    {
        if ( should_toggle )
        {
            toggle( setting );
        }
    }
};

/// \brief Creates an object that will ensure the provided setting is enabled (via glEnable)
///        and will reset the previous state on destruction.
/// \code
/// /* code that may or may not require blending */
/// {
///     auto scoped_blend = gl::scoped_enable(GL_BLEND);
///     /* code that requires blending */
///
///     // reset original blend state on destruction
/// }
/// /* code that may or may not require blending */
/// \endcode
/// \param setting - the state to enable
/// \param enable_setting - an optional parameter that can prevent this function from doing anything
[[nodiscard]] auto scoped_enable( GLenum setting, bool enable_setting = true )
    -> utils::GenericGuard< GLenumToggle, GLenumToggle >;

/// \brief Creates an object that will ensure the provided setting is enabled (via glEnable)
///        and will reset the previous state on destruction.
/// \code
/// /* code that may or may not require a depth test */
/// {
///     auto scoped_blend = gl::scoped_disable(GL_DEPTH_TEST);
///     /* code that requires no depth test */
///
///     // reset original depth test state on destruction
/// }
/// /* code that may or may not require a depth test */
/// \endcode
/// \param setting - the state to enable
/// \param disable_setting - an optional parameter that can prevent this function from doing anything
[[nodiscard]] auto scoped_disable( GLenum setting, bool disable_setting = true )
    -> utils::GenericGuard< GLenumToggle, GLenumToggle >;

/// \brief Functor that call an arbitrary function on a GLint
struct ValueSetter
{
    using Func = std::function< void( GLuint ) >;

    GLuint value;
    Func   reset_func;

    auto operator( )( ) const -> void { reset_func( value ); }
};

/// \brief Creates an object that will ensure the provided setting is set
///        and will reset the previous state on destruction.
///
/// Example usage:
/// \code
/// /* code that may be using another program */
/// {
///     auto scoped_program = gl::scoped_setting(GL_CURRENT_PROGRAM, 2, glUseProgram);
///     /* code that requires program 2 */
///
///     // reset original program state on destruction
/// }
/// /* code that may be using another program */
/// \endcode
/// \param setting - the state to enable
/// \param value - the value that we want to set the state to
/// \param setter - the function that actually sets the state
[[nodiscard]] auto scoped_setting( GLenum setting, GLuint value, ValueSetter::Func const& setter )
    -> utils::GenericGuard< ValueSetter, ValueSetter >;

/// \brief Functor that call an arbitrary function on a T[4] array
template < typename T >
struct ArraySetter
{
    using Func = std::function< void( T, T, T, T ) >;

    std::array< T, 4 > values;
    Func               reset_func;

    auto operator( )( ) const -> void { reset_func( values[ 0 ], values[ 1 ], values[ 2 ], values[ 3 ] ); }
};

/// \brief Creates an object that will ensure the provided setting is set
///        and will reset the previous state on destruction.
///
/// Example usage:
/// \code
/// /* code that may be using a different viewport */
/// {
///     auto scoped_viewport = gl::scoped_setting<GLint>(GL_VIEWPORT, {0, 0, 1200, 720}, glViewport);
///     /* code that requires the specified viewport */
///
///     // reset original viewport on destruction
/// }
/// /* code that may be using a different viewport */
/// \endcode
/// \param setting - the state to enable
/// \param values - the values that we want to set the state to
/// \param setter - the function that actually sets the state
template < typename T >
[[nodiscard]] auto
scoped_setting( GLenum setting, std::array< T, 4 > values, typename ArraySetter< T >::Func const& setter )
    -> utils::GenericGuard< ArraySetter< T >, ArraySetter< T > >
{
    auto set     = ArraySetter< T >{ values, setter };
    auto restore = ArraySetter< T >{ { }, setter };

    auto constexpr is_glint   = std::is_same_v< T, GLint >;
    auto constexpr is_glfloat = std::is_same_v< T, GLfloat >;

    static_assert( is_glint || is_glfloat );

    if constexpr ( is_glint )
    {
        glGetIntegerv( setting, restore.values.data( ) );
    }
    else if constexpr ( is_glfloat )
    {
        glGetFloatv( setting, restore.values.data( ) );
    }
    return utils::make_guard( set, std::move( restore ) );
}

} // namespace ltb::ogl
