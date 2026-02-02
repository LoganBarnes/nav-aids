
// project
// #include "ltb/utils/comparison_utils.hpp"
#include "ltb/aero/airspeed.hpp"
#include "ltb/aero/altitude.hpp"
#include "ltb/utils/types.hpp"

// external
#include <gtest/gtest.h>

namespace ltb
{
namespace
{

// True Airspeed (TAS) tests created using values from the CX-3 Flight Computer:
// https://online.prepware.com/cx3e/index.html

constexpr auto two_dimensional   = 2UL;
constexpr auto three_dimensional = 3UL;

template < typename T >
constexpr auto two_dimension_round_factor = T{ 100 };
template < typename T >
constexpr auto three_dimension_round_factor = T{ 1'000 };

// The CX-3 rounds mach to 3 decimal places.
template < size_t N, typename T >
constexpr auto round( T const value )
{
    if constexpr ( N == two_dimensional )
    {
        return std::round( value * two_dimension_round_factor< T > )
             / two_dimension_round_factor< T >;
    }
    else if constexpr ( N == three_dimensional )
    {
        return std::round( value * three_dimension_round_factor< T > )
             / three_dimension_round_factor< T >;
    }
    else
    {
        static_assert(
            ( N == two_dimensional ) || ( N == three_dimensional ),
            "Only 2 or 3 decimal places are supported."
        );
    }
}

// These are the parameters the CX-3 uses to calculate TAS.
template < typename T >
struct TestParams
{
    T cas_knots;
    T altitude_ft;
    T temperature_c;
};

template < typename T >
auto test_mach_number( TestParams< T > const& params )
{
    auto const cas_m_per_s        = utils::m_per_s_from_kts< T >( params.cas_knots );
    auto const altitude_m         = static_cast< T >( constants::m_from_ft ) * params.altitude_ft;
    auto const static_pressure_pa = aero::standard_pressure_at_altitude( altitude_m );

    return round< three_dimensional >( aero::mach_number( cas_m_per_s, static_pressure_pa ) );
}

template < typename T >
auto test_tas_knots( TestParams< T > const& params )
{
    auto const altitude_m         = static_cast< T >( constants::m_from_ft ) * params.altitude_ft;
    auto const static_pressure_pa = aero::standard_pressure_at_altitude( altitude_m );

    return round< two_dimensional >( aero::true_airspeed_knots(
        aero::TasKnotsParams< T >{
            .cas_knots          = params.cas_knots,
            .static_pressure_pa = static_pressure_pa,
            .temperature_c      = params.temperature_c,
        }
    ) );
}

template < typename T >
auto assert_eq( T const expected, T const actual )
{
    if constexpr ( std::is_same_v< T, float > )
    {
        ASSERT_FLOAT_EQ( expected, actual );
    }
    else if constexpr ( std::is_same_v< T, double > )
    {
        ASSERT_DOUBLE_EQ( expected, actual );
    }
    else
    {
        static_assert(
            std::is_same_v< T, float > || std::is_same_v< T, double >,
            "Only float and double are supported."
        );
    }
}

template < typename T >
class AirspeedTests : public ::testing::Test
{
};

using FloatTypes = ::testing::Types< float32, float64 >;
TYPED_TEST_SUITE( AirspeedTests, FloatTypes, );

TYPED_TEST( AirspeedTests, TasStandard )
{
    constexpr auto params = TestParams< TypeParam >{
        .cas_knots     = TypeParam{ 60 },
        .altitude_ft   = TypeParam{ 0 },
        .temperature_c = TypeParam{ 15 },
    };

    EXPECT_NO_FATAL_FAILURE( assert_eq( test_mach_number( params ), TypeParam( 0.091 ) ) );
    EXPECT_NO_FATAL_FAILURE( assert_eq( test_tas_knots( params ), TypeParam( 60.00 ) ) );
}

TYPED_TEST( AirspeedTests, TasStandardSlow )
{
    constexpr auto params = TestParams< TypeParam >{
        .cas_knots     = TypeParam{ 25 },
        .altitude_ft   = TypeParam{ 0 },
        .temperature_c = TypeParam{ 15 },
    };

    EXPECT_NO_FATAL_FAILURE( assert_eq( test_mach_number( params ), TypeParam( 0.038 ) ) );
    EXPECT_NO_FATAL_FAILURE( assert_eq( test_tas_knots( params ), TypeParam( 25.00 ) ) );
}

TYPED_TEST( AirspeedTests, TasStandardFast )
{
    constexpr auto params = TestParams< TypeParam >{
        .cas_knots     = TypeParam{ 130 },
        .altitude_ft   = TypeParam{ 0 },
        .temperature_c = TypeParam{ 15 },
    };

    EXPECT_NO_FATAL_FAILURE( assert_eq( test_mach_number( params ), TypeParam( 0.197 ) ) );
    EXPECT_NO_FATAL_FAILURE( assert_eq( test_tas_knots( params ), TypeParam( 130.00 ) ) );
}

TYPED_TEST( AirspeedTests, TasHigh )
{
    constexpr auto params = TestParams< TypeParam >{
        .cas_knots     = TypeParam{ 66 },
        .altitude_ft   = TypeParam{ 6500 },
        .temperature_c = TypeParam{ 15 },
    };

    EXPECT_NO_FATAL_FAILURE( assert_eq( test_mach_number( params ), TypeParam( 0.112 ) ) );
    EXPECT_NO_FATAL_FAILURE( assert_eq( test_tas_knots( params ), TypeParam( 74.40 ) ) );
}

TYPED_TEST( AirspeedTests, TasHighSlow )
{
    constexpr auto params = TestParams< TypeParam >{
        .cas_knots     = TypeParam{ 21 },
        .altitude_ft   = TypeParam{ 5762 },
        .temperature_c = TypeParam{ 15 },
    };

    EXPECT_NO_FATAL_FAILURE( assert_eq( test_mach_number( params ), TypeParam( 0.035 ) ) );
    EXPECT_NO_FATAL_FAILURE( assert_eq( test_tas_knots( params ), TypeParam( 23.35 ) ) );
}

TYPED_TEST( AirspeedTests, TasHighFast )
{
    constexpr auto params = TestParams< TypeParam >{
        .cas_knots     = TypeParam{ 123 },
        .altitude_ft   = TypeParam{ 8001 },
        .temperature_c = TypeParam{ 15 },
    };

    EXPECT_NO_FATAL_FAILURE( assert_eq( test_mach_number( params ), TypeParam( 0.215 ) ) );
    EXPECT_NO_FATAL_FAILURE( assert_eq( test_tas_knots( params ), TypeParam( 142.51 ) ) );
}

TYPED_TEST( AirspeedTests, TasHighHotSlow )
{
    constexpr auto params = TestParams< TypeParam >{
        .cas_knots     = TypeParam{ 16 },
        .altitude_ft   = TypeParam{ 7777 },
        .temperature_c = TypeParam{ 40 },
    };

    EXPECT_NO_FATAL_FAILURE( assert_eq( test_mach_number( params ), TypeParam( 0.028 ) ) );
    EXPECT_NO_FATAL_FAILURE( assert_eq( test_tas_knots( params ), TypeParam( 19.27 ) ) );
}

TYPED_TEST( AirspeedTests, TasHighHotFast )
{
    constexpr auto params = TestParams< TypeParam >{
        .cas_knots     = TypeParam{ 111 },
        .altitude_ft   = TypeParam{ 6912 },
        .temperature_c = TypeParam{ 36 },
    };

    EXPECT_NO_FATAL_FAILURE( assert_eq( test_mach_number( params ), TypeParam( 0.191 ) ) );
    EXPECT_NO_FATAL_FAILURE( assert_eq( test_tas_knots( params ), TypeParam( 130.54 ) ) );
}

TYPED_TEST( AirspeedTests, TasLowCoolSlow )
{
    constexpr auto params = TestParams< TypeParam >{
        .cas_knots     = TypeParam{ 40 },
        .altitude_ft   = TypeParam{ 1200 },
        .temperature_c = TypeParam{ 0 },
    };

    EXPECT_NO_FATAL_FAILURE( assert_eq( test_mach_number( params ), TypeParam( 0.062 ) ) );
    EXPECT_NO_FATAL_FAILURE( assert_eq( test_tas_knots( params ), TypeParam( 39.80 ) ) );
}

TYPED_TEST( AirspeedTests, TasLowCoolFast )
{
    constexpr auto params = TestParams< TypeParam >{
        .cas_knots     = TypeParam{ 115 },
        .altitude_ft   = TypeParam{ -200 },
        .temperature_c = TypeParam{ -7 },
    };

    EXPECT_NO_FATAL_FAILURE( assert_eq( test_mach_number( params ), TypeParam( 0.173 ) ) );
    EXPECT_NO_FATAL_FAILURE( assert_eq( test_tas_knots( params ), TypeParam( 110.13 ) ) );
}

} // namespace
} // namespace ltb
