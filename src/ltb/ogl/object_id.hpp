#pragma once

// graphics
#include "opengl.hpp"

// project
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/utils.hpp"

namespace ltb::ogl
{

class ObjectId
{
public:
    explicit ObjectId( GLuint id, );

    /// \brief The original object.
    [[nodiscard( "Const getter" )]]
    auto object( ) const -> OglObj const&;

    /// \brief The type used when binding this object.
    [[nodiscard( "Const getter" )]]
    auto binding_type( ) const -> GLenum;

private:
    OglObj& obj_;

    std::shared_ptr< void > restore_value_callback_ = nullptr;

    struct RestoreValue
    {
        WhenFinished when_finished_;
        GLint        restore_value_;

        template < typename Ignored >
        auto operator( )( Ignored const* ignored ) const -> void;
    };
};

template < GLenum BindingType, typename OglObj >
auto bind( OglObj& object, WhenFinished when_finished )
{
    return ObjectId< OglObj, BindingType >( object, when_finished );
}

template < GLenum BindingType, typename OglObj >
auto bind( OglObj& object )
{
    return make_bound< BindingType >( object, WhenFinished::RestoreNullState );
}

template < typename OglObj, GLenum BindingType >
ObjectId< OglObj, BindingType >::ObjectId( OglObj& obj, WhenFinished when_finished )
    : obj_( obj )
{
    auto restore_value = GLint{ 0 };

    switch ( when_finished )
    {
        using enum WhenFinished;

        case RestorePreviousState: {
            glGetIntegerv( ogl::binding_getter_type< BindingType >, &restore_value );
        }
            // fallthrough
        case RestoreNullState:
            restore_value_callback_
                = std::shared_ptr< void >( this, RestoreValue{ when_finished, restore_value } );
            break;

        case DoNothing:
            break;

    } // end switch

    OglObj::static_bind( BindingType, obj_ );
}

template < typename OglObj, GLenum BindingType >
template < typename Ignored >
auto ObjectId< OglObj, BindingType >::RestoreValue::operator( )( Ignored const* ) const -> void
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
auto ObjectId< OglObj, BindingType >::object( ) const -> OglObj const&
{
    return obj_;
}

template < typename OglObj, GLenum BindingType >
auto ObjectId< OglObj, BindingType >::binding_type( ) const -> GLenum
{
    return BindingType;
}

} // namespace ltb::ogl
