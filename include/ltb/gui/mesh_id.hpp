#pragma once

// project
#include "ltb/gui/incremental_id_generator.hpp"
#include "ltb/utils/types.hpp"

// standard
#include <compare>
#include <string> // std::hash

namespace ltb::gui
{

/// \brief A unique identifier for an allocated piece of geometry.
template < typename Type >
class TypeId
{
public:
    [[nodiscard( "Const Getter" )]] auto id( ) const -> uint32;
    [[nodiscard( "Const Getter" )]] auto is_nil( ) const -> bool;

    auto operator<=>( TypeId< Type > const& ) const = default;

    static auto nil( ) -> TypeId< Type >;

private:
    friend class IncrementalIdGenerator< uint32 >;

    explicit constexpr TypeId( uint32 id );

    uint32 id_;

    constexpr static auto nil_value_ = 0U;
};

template < typename Type >
constexpr TypeId< Type >::TypeId( uint32 const id )
    : id_{ id }
{
}

template < typename Type >
auto TypeId< Type >::id( ) const -> uint32_t
{
    return id_;
}

template < typename Type >
auto TypeId< Type >::is_nil( ) const -> bool
{
    return nil_value_ != id_;
}

template < typename Type >
auto TypeId< Type >::nil( ) -> TypeId< Type >
{
    return TypeId< Type >{ nil_value_ };
}

} // namespace ltb::gui

namespace std
{

template < typename Type >
struct hash< ltb::gui::TypeId< Type > >
{
    auto operator( )( ltb::gui::TypeId< Type > const& id ) const noexcept
    {
        return hash< ltb::uint32 >{ }( id.id( ) );
    }
};

} // namespace std
