#pragma once

// graphics
#include "opengl.hpp"

// project
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/utils.hpp"

namespace ltb::ogl
{

template < typename OglObj, GLenum BindingType >
class Bound
{
public:
    explicit Bound( OglObj& obj, WhenFinished when_finished );

    /// \brief The original object.
    [[nodiscard]] auto object( ) const -> OglObj const&;

    /// \brief The type used when binding this object.
    [[nodiscard]] auto binding_type( ) const -> GLenum;

private:
    OglObj& obj_;

    std::shared_ptr< void > restore_value_callback_ = nullptr;

    struct RestoreValue
    {
        WhenFinished when_finished_;
        GLint        restore_value_;

        template < typename Ignored >
        auto operator( )( Ignored const* ) const -> void;
    };
};

template < typename OglObj, GLenum BindingType >
Bound< OglObj, BindingType >::Bound( OglObj& obj, WhenFinished when_finished )
    : obj_( obj )
{
    auto restore_value = GLint{ 0 };

    switch ( when_finished )
    {
        using enum WhenFinished;

        case RestorePreviousState:
            glGetIntegerv( ogl::binding_type( BindingType ), &restore_value );
            // fallthrough
        case RestoreNullState:
            restore_value_callback_ = std::shared_ptr< void >( this, RestoreValue{ when_finished, restore_value } );
            break;

        case DoNothing:
            break;

    } // end switch

    OglObj::static_bind( BindingType, obj_ );
}

template < typename OglObj, GLenum BindingType >
template < typename Ignored >
auto Bound< OglObj, BindingType >::RestoreValue::operator( )( Ignored const* ) const -> void
{
    switch ( when_finished_ )
    {
        using enum WhenFinished;

        case RestorePreviousState:
        case RestoreNullState:
            OglObj::static_bind( BindingType, static_cast< GLuint >( restore_value_ ) );
            break;

        case DoNothing:
            break;

    } // end switch
}

template < typename OglObj, GLenum BindingType >
auto Bound< OglObj, BindingType >::object( ) const -> OglObj const&
{
    return obj_;
}

template < typename OglObj, GLenum BindingType >
auto Bound< OglObj, BindingType >::binding_type( ) const -> GLenum
{
    return BindingType;
}

} // namespace ltb::ogl
