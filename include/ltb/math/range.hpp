#pragma once

// project
#include "ltb/math/fwd.hpp"

namespace ltb::math
{

template < typename T >
struct Range
{
    T min = { };
    T max = { };
};

template < typename T >
constexpr auto dimensions( Range< T > const& range )
{
    return range.max - range.min;
}

template < typename T >
constexpr auto center( Range< T > const& range )
{
    constexpr auto two = T{ 2 };
    // Find the center in a way that avoids overflow.
    return ( range.min / two ) + ( range.max / two );
}

template < glm::length_t L, typename T >
constexpr auto contains( Range< glm::vec< L, T > > const& range, glm::vec< L, T > const& p )
{
    for ( auto i = glm::length_t{ 0 }; i < L; ++i )
    {
        if ( ( range.min[ i ] > p[ i ] ) || ( p[ i ] > range.max[ i ] ) )
        {
            return false;
        }
    }
    return true;
}

template < typename T >
constexpr auto contains( Range< T > const& range, T v )
{
    return ( range.min <= v ) && ( v <= range.max );
}

template < typename T >
constexpr auto lerp( T const a, T const b, T const t )
{
    return a + ( t * ( b - a ) );
}

/// \brief Find the interpolant 't' for a value between \p a
///        and \p b such that \p v = lerp(a, b, t).
template < typename T >
constexpr auto inv_lerp( T const a, T const b, T const v ) -> T
{
    return ( v - a ) / ( b - a );
}

template < typename T >
constexpr auto
to_new_range( T const old_value, Range< T > const& old_range, Range< T > const& new_range )
{
    auto const interpolant = inv_lerp( old_range.min, old_range.max, old_value );
    auto const new_value   = lerp( new_range.min, new_range.max, interpolant );
    return new_value;
}

} // namespace ltb::math
