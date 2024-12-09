
// project
#include "ltb/math/transforms.hpp"

// external
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <gtest/gtest.h>

namespace ltb
{
namespace
{

TEST( TransformsTests, ConsolidateTransforms2d )
{
    constexpr auto scale1       = glm::vec2{ 2.0F, 0.5F };
    constexpr auto rotation1    = glm::radians( 90.0F );
    constexpr auto translation1 = glm::vec2{ 1.0F, 2.0F };

    constexpr auto scale2       = glm::vec2{ -0.25F, 1.5F };
    constexpr auto rotation2    = glm::radians( -195.0F );
    constexpr auto translation2 = glm::vec2{ -5.0F, +42.0F };

    auto const expected =
        // Multiplication order should be opposite the order
        // of the transforms in the vector.
        glm::translate( glm::identity< glm::mat3 >( ), translation2 )
        * glm::scale( glm::identity< glm::mat3 >( ), scale2 )
        * glm::rotate( glm::identity< glm::mat3 >( ), rotation2 )
        * glm::scale( glm::identity< glm::mat3 >( ), scale1 )
        * glm::rotate( glm::identity< glm::mat3 >( ), rotation1 )
        * glm::translate( glm::identity< glm::mat3 >( ), translation1 );

    auto const transforms = std::vector< math::Transform2d >{
        math::Translation2d{ translation1 },
        math::Rotation2d{ rotation1 },
        math::Scale2d{ scale1 },
        math::Rotation2d{ rotation2 },
        math::Scale2d{ scale2 },
        math::Translation2d{ translation2 },
    };

    auto const actual = math::consolidate_transforms( transforms );

    EXPECT_FLOAT_EQ( actual[ 0 ][ 0 ], expected[ 0 ][ 0 ] );
    EXPECT_FLOAT_EQ( actual[ 0 ][ 1 ], expected[ 0 ][ 1 ] );
    EXPECT_FLOAT_EQ( actual[ 0 ][ 2 ], expected[ 0 ][ 2 ] );
    EXPECT_FLOAT_EQ( actual[ 1 ][ 0 ], expected[ 1 ][ 0 ] );
    EXPECT_FLOAT_EQ( actual[ 1 ][ 1 ], expected[ 1 ][ 1 ] );
    EXPECT_FLOAT_EQ( actual[ 1 ][ 2 ], expected[ 1 ][ 2 ] );
    EXPECT_FLOAT_EQ( actual[ 2 ][ 0 ], expected[ 2 ][ 0 ] );
    EXPECT_FLOAT_EQ( actual[ 2 ][ 1 ], expected[ 2 ][ 1 ] );
    EXPECT_FLOAT_EQ( actual[ 2 ][ 2 ], expected[ 2 ][ 2 ] );
}

TEST( TransformsTests, ConsolidateTransforms3d )
{
    constexpr auto scale1       = glm::vec3{ 2.0F, 0.5F, 1.2F };
    constexpr auto axis1        = glm::vec3{ 1.0F, 1.0F, 1.0F };
    constexpr auto rotation1    = glm::radians( 90.0F );
    constexpr auto translation1 = glm::vec3{ 1.0F, -2.0F, 3.0F };

    constexpr auto scale2       = glm::vec3{ -0.25F, 1.5F, 0.8F };
    constexpr auto axis2        = glm::vec3{ -1.0F, 0.5F, 0.0F };
    constexpr auto rotation2    = glm::radians( -195.0F );
    constexpr auto translation2 = glm::vec3{ -5.0F, 0.0F, +42.0F };

    auto const expected =
        // Multiplication order should be opposite the order
        // of the transforms in the vector.
        glm::translate( glm::identity< glm::mat4 >( ), translation2 )
        * glm::scale( glm::identity< glm::mat4 >( ), scale2 )
        * glm::rotate( glm::identity< glm::mat4 >( ), rotation2, axis2 )
        * glm::scale( glm::identity< glm::mat4 >( ), scale1 )
        * glm::rotate( glm::identity< glm::mat4 >( ), rotation1, axis1 )
        * glm::translate( glm::identity< glm::mat4 >( ), translation1 );

    auto const transforms = std::vector< math::Transform3d >{
        math::Translation3d{ translation1 },
        math::Rotation3d{ rotation1, axis1 },
        math::Scale3d{ scale1 },
        math::Rotation3d{ rotation2, axis2 },
        math::Scale3d{ scale2 },
        math::Translation3d{ translation2 },
    };

    auto const actual = math::consolidate_transforms( transforms );

    EXPECT_FLOAT_EQ( actual[ 0 ][ 0 ], expected[ 0 ][ 0 ] );
    EXPECT_FLOAT_EQ( actual[ 0 ][ 1 ], expected[ 0 ][ 1 ] );
    EXPECT_FLOAT_EQ( actual[ 0 ][ 2 ], expected[ 0 ][ 2 ] );
    EXPECT_FLOAT_EQ( actual[ 0 ][ 3 ], expected[ 0 ][ 3 ] );
    EXPECT_FLOAT_EQ( actual[ 1 ][ 0 ], expected[ 1 ][ 0 ] );
    EXPECT_FLOAT_EQ( actual[ 1 ][ 1 ], expected[ 1 ][ 1 ] );
    EXPECT_FLOAT_EQ( actual[ 1 ][ 2 ], expected[ 1 ][ 2 ] );
    EXPECT_FLOAT_EQ( actual[ 1 ][ 3 ], expected[ 1 ][ 3 ] );
    EXPECT_FLOAT_EQ( actual[ 2 ][ 0 ], expected[ 2 ][ 0 ] );
    EXPECT_FLOAT_EQ( actual[ 2 ][ 1 ], expected[ 2 ][ 1 ] );
    EXPECT_FLOAT_EQ( actual[ 2 ][ 2 ], expected[ 2 ][ 2 ] );
    EXPECT_FLOAT_EQ( actual[ 2 ][ 3 ], expected[ 2 ][ 3 ] );
    EXPECT_FLOAT_EQ( actual[ 3 ][ 0 ], expected[ 3 ][ 0 ] );
    EXPECT_FLOAT_EQ( actual[ 3 ][ 1 ], expected[ 3 ][ 1 ] );
    EXPECT_FLOAT_EQ( actual[ 3 ][ 2 ], expected[ 3 ][ 2 ] );
    EXPECT_FLOAT_EQ( actual[ 3 ][ 3 ], expected[ 3 ][ 3 ] );
}

} // namespace
} // namespace ltb
