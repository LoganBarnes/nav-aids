#pragma once

// external
#include <glm/glm.hpp>

namespace ltb::math
{

template < typename T >
struct Range
{
    T min = { };
    T max = { };
};

using Range2Di = Range< glm::ivec2 >;
using Range2D  = Range< glm::vec2 >;

template < typename T >
constexpr auto dimensions( Range< T > const& range )
{
    return range.max - range.min;
}

template < typename T >
constexpr auto center( Range< T > const& range )
{
    // Find the center in a way that avoids overflow.
    return range.min / T( 2 ) + range.max / T( 2 );
}

template < glm::length_t L, typename T >
constexpr auto contains( Range< glm::vec< L, T > > const& range, glm::vec< L, T > const& p )
{
    for ( decltype( L ) i = 0; i < L; ++i )
    {
        if ( range.min[ i ] > p[ i ] || p[ i ] > range.max[ i ] )
        {
            return false;
        }
    }
    return true;
}

template < typename T >
constexpr auto contains( Range< T > const& range, T v )
{
    return range.min <= v && v <= range.max;
}

} // namespace ltb::math
