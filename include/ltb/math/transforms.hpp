#pragma once

// project
#include "ltb/math/constants.hpp"
#include "ltb/utils/types.hpp"

// external
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

// standard
#include <variant>
#include <vector>

namespace ltb::math
{

constexpr auto two_dimensions   = glm::length_t{ 2 };
constexpr auto three_dimensions = glm::length_t{ 3 };

template < glm::length_t Dimensions >
concept TwoOrThreeD = ( two_dimensions == Dimensions ) || ( three_dimensions == Dimensions );

template < glm::length_t Dimensions >
    requires TwoOrThreeD< Dimensions >
struct Translation;

template < glm::length_t Dimensions >
    requires TwoOrThreeD< Dimensions >
struct Rotation;

template < glm::length_t Dimensions >
    requires TwoOrThreeD< Dimensions >
struct Scale;

template < glm::length_t Dimensions >
    requires TwoOrThreeD< Dimensions >
using Transform
    = std::variant< Translation< Dimensions >, Rotation< Dimensions >, Scale< Dimensions > >;

// 2D
using Translation2d = Translation< two_dimensions >;
using Rotation2d    = Rotation< two_dimensions >;
using Scale2d       = Scale< two_dimensions >;
using Transform2d   = Transform< two_dimensions >;

// 3D
using Translation3d = Translation< three_dimensions >;
using Rotation3d    = Rotation< three_dimensions >;
using Scale3d       = Scale< three_dimensions >;
using Transform3d   = Transform< three_dimensions >;

template < glm::length_t Dimensions >
using Mat = glm::mat< Dimensions, Dimensions, float32 >;

/// \brief Consolidates a collection of 2D transforms into a single 2D transformation matrix.
///        Transforms will be applied in the order they are provided:
///        T_final = T_n * T_n-1 * ... * T_1 * T_0
template < glm::length_t Dimensions >
auto consolidate_transforms( std::vector< Transform< Dimensions > > const& transforms )
    -> Mat< Dimensions + 1 >;

/// \brief Converts a 3D world transformation matrix to a 3x3 matrix for transforming normals.
auto normals_transform( glm::mat4 const& world_from_local ) -> glm::mat3;

// Convert transforms to homogeneous transformation matrices:

template < glm::length_t Dimensions >
    requires TwoOrThreeD< Dimensions >
auto to_mat( Translation< Dimensions > const& translation ) -> Mat< Dimensions + 1 >;

template < glm::length_t Dimensions >
    requires TwoOrThreeD< Dimensions >
auto to_mat( Rotation< Dimensions > const& rotation ) -> Mat< Dimensions + 1 >;

template < glm::length_t Dimensions >
    requires TwoOrThreeD< Dimensions >
auto to_mat( Scale< Dimensions > const& scale ) -> Mat< Dimensions + 1 >;

// ////////////////// Transform Implementations Below ////////////////// //

template < glm::length_t Dimensions >
    requires TwoOrThreeD< Dimensions >
struct Translation
{
    glm::vec< Dimensions, float32 > offset = glm::vec< Dimensions, float32 >( 0.0F );
};

template <>
struct Rotation< two_dimensions >
{
    float32 angle_radians = 0.0F;
};

template <>
struct Rotation< three_dimensions >
{
    float32   angle_radians = 0.0F;
    glm::vec3 axis          = z_axis3;
};

template < glm::length_t Dimensions >
    requires TwoOrThreeD< Dimensions >
struct Scale
{
    glm::vec< Dimensions, float32 > scale = glm::vec< Dimensions, float32 >( 1.0F );
};

} // namespace ltb::math
