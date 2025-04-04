#pragma once

// standard
#include <type_traits>

namespace ltb::utils
{

template < typename E, std::enable_if_t< std::is_enum_v< E >, int > = 0 >
constexpr auto to_underlying( E const& value )
{
    return static_cast< std::underlying_type_t< E > >( value );
}

template < typename E, std::enable_if_t< std::is_enum_v< E >, int > = 0 >
constexpr auto from_underlying( std::underlying_type_t< E > const& value ) -> E
{
    return static_cast< E >( value );
}

/// \brief Converts incremental enums to bit flag values
///
/// \code
/// enum class Enum : int {
///     first,
///     second,
///     third
/// }
///
/// to_bit_flag(Enum::first);  // -> 0b0001
/// to_bit_flag(Enum::second); // -> 0b0010
/// to_bit_flag(Enum::third);  // -> 0b0100
/// \endcode
template < typename E, std::enable_if_t< std::is_enum_v< E >, int > = 0 >
constexpr auto to_bit_flag( E type ) -> std::underlying_type_t< E >
{
    // In C++, unsigned types less than 32 bits are
    // promoted to int32_t when used in a shift operation.
    static_assert( std::is_same_v< decltype( uint16_t{ 1 } << 1u ), int32_t > );

    // Therefore, we explicitly cast back to type U
    // to avoid signed/unsigned compiler warnings.
    using U = std::underlying_type_t< E >;
    return static_cast< U >( U( 1 ) << to_underlying( type ) );
}

} // namespace ltb::utils
