#pragma once

// project
#include "ltb/utils/enum_utils.hpp"

// external
#include <magic_enum.hpp>

// standard
#include <limits>

namespace ltb::utils
{

template < typename E >
struct Flags
{
    using Type = std::underlying_type_t< E >;

    static_assert( std::is_enum_v< E > );
    static_assert( magic_enum::enum_count< E >( ) <= std::numeric_limits< Type >::digits );

    Flags( ) = default;

    Flags( E initial_flag )
        : bits( to_bit_flag( initial_flag ) )
    {
    } // NOLINT(google-explicit-constructor)

    explicit Flags( Type initial_bits )
        : bits( initial_bits )
    {
    }

    operator bool( ) const { return bits != 0; } // NOLINT(google-explicit-constructor)

    Type bits = Type( 0 );
};

template < typename E >
auto operator==( Flags< E > lhs, Flags< E > rhs ) -> bool
{
    return lhs.bits == rhs.bits;
}

template < typename E >
auto operator!=( Flags< E > lhs, Flags< E > rhs ) -> bool
{
    return !( lhs == rhs );
}

namespace flag_operators
{

// ~
template < typename E >
constexpr auto operator~( Flags< E > flags ) noexcept -> Flags< E >
{
    return Flags< E >{ static_cast< typename Flags< E >::Type >( ~flags.bits ) };
}

template < typename E >
constexpr auto operator~( E value ) noexcept -> Flags< E >
{
    return ~Flags< E >{ value };
}

// |
template < typename E >
constexpr auto operator|( Flags< E > lhs, Flags< E > rhs ) noexcept -> Flags< E >
{
    return Flags< E >{ static_cast< typename Flags< E >::Type >( lhs.bits | rhs.bits ) };
}

template < typename E >
constexpr auto operator|( Flags< E > lhs, E rhs ) noexcept -> Flags< E >
{
    return ( lhs | Flags< E >{ rhs } );
}

template < typename E >
constexpr auto operator|( E lhs, Flags< E > rhs ) noexcept -> Flags< E >
{
    return ( Flags< E >{ lhs } | rhs );
}

template < typename E >
constexpr auto operator|( E lhs, E rhs ) noexcept -> Flags< E >
{
    return ( Flags< E >{ lhs } | Flags< E >{ rhs } );
}

// &
template < typename E >
constexpr auto operator&( Flags< E > lhs, Flags< E > rhs ) noexcept -> Flags< E >
{
    return Flags< E >{ static_cast< typename Flags< E >::Type >( lhs.bits & rhs.bits ) };
}

template < typename E >
constexpr auto operator&( Flags< E > lhs, E rhs ) noexcept -> Flags< E >
{
    return ( lhs & Flags< E >{ rhs } );
}

template < typename E >
constexpr auto operator&( E lhs, Flags< E > rhs ) noexcept -> Flags< E >
{
    return ( Flags< E >{ lhs } & rhs );
}

template < typename E >
constexpr auto operator&( E lhs, E rhs ) noexcept -> Flags< E >
{
    return ( Flags< E >{ lhs } & Flags< E >{ rhs } );
}

// ^
template < typename E >
constexpr auto operator^( Flags< E > lhs, Flags< E > rhs ) noexcept -> Flags< E >
{
    return Flags< E >{ static_cast< typename Flags< E >::Type >( lhs.bits ^ rhs.bits ) };
}

template < typename E >
constexpr auto operator^( Flags< E > lhs, E rhs ) noexcept -> Flags< E >
{
    return ( lhs ^ Flags< E >{ rhs } );
}

template < typename E >
constexpr auto operator^( E lhs, Flags< E > rhs ) noexcept -> Flags< E >
{
    return ( Flags< E >{ lhs } ^ rhs );
}

template < typename E >
constexpr auto operator^( E lhs, E rhs ) noexcept -> Flags< E >
{
    return ( Flags< E >{ lhs } ^ Flags< E >{ rhs } );
}

// |=
template < typename E >
constexpr auto operator|=( Flags< E >& lhs, Flags< E > rhs ) noexcept -> Flags< E >&
{
    return lhs = ( lhs | rhs );
}

template < typename E >
constexpr auto operator|=( Flags< E >& lhs, E rhs ) noexcept -> Flags< E >&
{
    return lhs = ( lhs | Flags< E >{ rhs } );
}

// &=
template < typename E >
constexpr auto operator&=( Flags< E >& lhs, Flags< E > rhs ) noexcept -> Flags< E >&
{
    return lhs = ( lhs & rhs );
}

template < typename E >
constexpr auto operator&=( Flags< E >& lhs, E rhs ) noexcept -> Flags< E >&
{
    return lhs = ( lhs & Flags< E >{ rhs } );
}

// ^=
template < typename E >
constexpr auto operator^=( Flags< E >& lhs, Flags< E > rhs ) noexcept -> Flags< E >&
{
    return lhs = ( lhs ^ rhs );
}

template < typename E >
constexpr auto operator^=( Flags< E >& lhs, E rhs ) noexcept -> Flags< E >&
{
    return lhs = ( lhs ^ Flags< E >{ rhs } );
}

} // namespace flag_operators

template < typename E, typename... Es >
constexpr auto make_flags( E flag ) -> Flags< E >
{
    using namespace flag_operators;
    return Flags< E >{ flag };
}

template < typename E, typename... Es >
constexpr auto make_flags( E flag, Es... flags ) -> Flags< E >
{
    using namespace flag_operators;
    return make_flags< E >( flag ) | make_flags< E >( std::forward< Es >( flags )... );
}

template < typename E >
constexpr auto add_flag( Flags< E > flags, E flag ) -> Flags< E >
{
    using namespace flag_operators;
    return flags | flag;
}

template < typename E >
constexpr auto remove_flag( Flags< E > flags, E flag ) -> Flags< E >
{
    using namespace flag_operators;
    return flags & ~flag;
}

template < typename E >
constexpr auto toggle_flag( Flags< E > flags, E flag ) -> Flags< E >
{
    using namespace flag_operators;
    return flags ^ flag;
}

template < typename E >
constexpr auto has_flag( Flags< E > flags, E flag ) -> bool
{
    using namespace flag_operators;
    return flags & flag;
}

template < typename E >
constexpr auto no_flags( ) -> Flags< E >
{
    using namespace flag_operators;
    return Flags< E >{ };
}

template < typename E >
constexpr auto all_flags( ) -> Flags< E >
{
    using namespace flag_operators;
    return ~no_flags< E >( );
}

} // namespace ltb::utils
