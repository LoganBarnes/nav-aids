#pragma once

// standard
#include <cstdlib>
#include <type_traits>

namespace ltb::utils
{

/// \brief Convert a 2D size to a 1D size.
template < typename T >
    requires std::is_integral_v< T >
constexpr auto total_size( T width, T height ) -> std::size_t
{
    return static_cast< std::size_t >( width ) * static_cast< std::size_t >( height );
}

/// \brief Convert a 2D index to a 1D index.
template < typename T >
    requires std::is_integral_v< T >
constexpr auto array_index( T x, T y, T width ) -> std::size_t
{
    return static_cast< std::size_t >( y ) * static_cast< std::size_t >( width )
         + static_cast< std::size_t >( x );
}

} // namespace ltb::utils
