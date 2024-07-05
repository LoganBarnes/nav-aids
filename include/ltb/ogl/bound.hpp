#pragma once

// graphics
#include "opengl.hpp"

// project
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/utils.hpp"

namespace ltb::ogl
{

template < typename OglObj, GLenum bind_type = GL_INVALID_ENUM >
class Bound;

template < typename OglObj, GLenum bind_type >
class Bound
{
public:
    explicit Bound( OglObj const& obj, WhenFinished when_finished );

    /// \brief The original object.
    [[nodiscard( "Const getter" )]]
    auto object( ) const -> OglObj const&;

private:
    OglObj const& obj_;

    std::shared_ptr< void > restore_value_callback_ = nullptr;

    struct RestoreValue
    {
        WhenFinished when_finished_;
        GLint        restore_value_;

        template < typename Ignored >
        auto operator( )( Ignored const* ignored ) const -> void;
    };
};

template < typename OglObj >
class Bound< OglObj, GL_INVALID_ENUM >
{
public:
    explicit Bound( OglObj const& obj, WhenFinished when_finished );

    /// \brief The original object.
    [[nodiscard( "Const getter" )]]
    auto object( ) const -> OglObj const&;

private:
    OglObj const& obj_;

    std::shared_ptr< void > restore_value_callback_ = nullptr;

    struct RestoreValue
    {
        WhenFinished when_finished_;
        GLint        restore_value_;

        template < typename Ignored >
        auto operator( )( Ignored const* ignored ) const -> void;
    };
};

template < GLenum bind_type, typename OglObj >
auto bind( OglObj const& object, WhenFinished when_finished )
{
    return Bound< OglObj, bind_type >( object, when_finished );
}

template < GLenum bind_type, typename OglObj >
auto bind( OglObj const& object )
{
    return bind< bind_type >( object, WhenFinished::RestoreNullState );
}

template < typename OglObj >
auto bind( OglObj const& object, WhenFinished when_finished )
{
    return Bound< OglObj, GL_INVALID_ENUM >( object, when_finished );
}

template < typename OglObj >
auto bind( OglObj const& object )
{
    return bind< OglObj >( object, WhenFinished::RestoreNullState );
}

template < typename OglObj, GLenum bind_type >
Bound< OglObj, bind_type >::Bound( OglObj const& obj, WhenFinished when_finished )
    : obj_( obj )
{
    auto restore_value = GLint{ 0 };

    switch ( when_finished )
    {
        using enum WhenFinished;

        case RestorePreviousState: {
            glGetIntegerv( ogl::binding_getter_type< bind_type >( ), &restore_value );
        }
            // fallthrough
        case RestoreNullState:
            restore_value_callback_
                = std::shared_ptr< void >( this, RestoreValue{ when_finished, restore_value } );
            break;

        case DoNothing:
            break;

    } // end switch

    OglObj::static_bind( bind_type, obj_ );
}

template < typename OglObj, GLenum bind_type >
template < typename Ignored >
auto Bound< OglObj, bind_type >::RestoreValue::operator( )( Ignored const* ) const -> void
{
    switch ( when_finished_ )
    {
        using enum WhenFinished;

        case RestorePreviousState:
        case RestoreNullState:
            OglObj::static_bind( bind_type, static_cast< GLuint >( restore_value_ ) );
            break;

        case DoNothing:
            break;

    } // end switch
}

template < typename OglObj, GLenum bind_type >
auto Bound< OglObj, bind_type >::object( ) const -> OglObj const&
{
    return obj_;
}

template < typename OglObj >
Bound< OglObj, GL_INVALID_ENUM >::Bound( OglObj const& obj, WhenFinished when_finished )
    : obj_( obj )
{
    auto restore_value = GLint{ 0 };

    switch ( when_finished )
    {
        using enum WhenFinished;

        case RestorePreviousState: {
            glGetIntegerv( ogl::binding_getter_type< OglObj >( ), &restore_value );
        }
            // fallthrough
        case RestoreNullState:
            restore_value_callback_
                = std::shared_ptr< void >( this, RestoreValue{ when_finished, restore_value } );
            break;

        case DoNothing:
            break;

    } // end switch

    OglObj::static_bind( obj_ );
}

template < typename OglObj >
template < typename Ignored >
auto Bound< OglObj, GL_INVALID_ENUM >::RestoreValue::operator( )( Ignored const* ) const -> void
{
    switch ( when_finished_ )
    {
        using enum WhenFinished;

        case RestorePreviousState:
        case RestoreNullState:
            OglObj::static_bind( static_cast< GLuint >( restore_value_ ) );
            break;

        case DoNothing:
            break;

    } // end switch
}

template < typename OglObj >
auto Bound< OglObj, GL_INVALID_ENUM >::object( ) const -> OglObj const&
{
    return obj_;
}

} // namespace ltb::ogl
