#include "ltb/math/transforms.hpp"

// external
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <spdlog/spdlog.h>

// standard
#include <numeric>

namespace ltb::math
{
namespace
{

struct TransformToMatVisitor
{
    auto operator( )( auto const& transform ) const { return to_mat( transform ); }
};

struct TransformToMat
{
    template < glm::length_t Dimensions >
    auto operator( )( Transform< Dimensions > const& transform ) const
    {
        return std::visit( TransformToMatVisitor{ }, transform );
    }
};

} // namespace

template < glm::length_t Dimensions >
auto consolidate_transforms( std::vector< Transform< Dimensions > > const& transforms )
    -> Mat< Dimensions + 1 >
{
    using Matrix = Mat< Dimensions + 1 >;

    constexpr auto initial_value       = glm::identity< Matrix >( );
    constexpr auto transform_operation = TransformToMat{ };
    constexpr auto reduce_operation    = std::multiplies< Matrix >{ };

    return std::transform_reduce(
        transforms.rbegin( ),
        transforms.rend( ),
        initial_value,
        reduce_operation,
        transform_operation
    );
}

template auto consolidate_transforms( std::vector< Transform2d > const& transforms ) -> glm::mat3;
template auto consolidate_transforms( std::vector< Transform3d > const& transforms ) -> glm::mat4;

template < glm::length_t Dimensions >
    requires TwoOrThreeD< Dimensions >
auto to_mat( Translation< Dimensions > const& translation ) -> Mat< Dimensions + 1 >
{
    return glm::translate( glm::identity< Mat< Dimensions + 1 > >( ), translation.offset );
}

template < glm::length_t Dimensions >
    requires TwoOrThreeD< Dimensions >
auto to_mat( Rotation< Dimensions > const& rotation ) -> Mat< Dimensions + 1 >
{
    using Matrix = Mat< Dimensions + 1 >;

    if constexpr ( Dimensions == two_dimensions )
    {
        return glm::rotate( glm::identity< Matrix >( ), rotation.angle_radians );
    }
    else if ( Dimensions == three_dimensions )
    {
        return glm::rotate( glm::identity< Matrix >( ), rotation.angle_radians, rotation.axis );
    }
    else
    {
        static_assert( TwoOrThreeD< Dimensions >, "Dimensions must be 2 or 3" );
    }
}

template < glm::length_t Dimensions >
    requires TwoOrThreeD< Dimensions >
auto to_mat( Scale< Dimensions > const& scale ) -> Mat< Dimensions + 1 >
{
    return glm::scale( glm::identity< Mat< Dimensions + 1 > >( ), scale.scale );
}

// 2D
template auto to_mat( Translation2d const& translation ) -> glm::mat3;
template auto to_mat( Rotation2d const& translation ) -> glm::mat3;
template auto to_mat( Scale2d const& translation ) -> glm::mat3;

// 3D
template auto to_mat( Translation3d const& translation ) -> glm::mat4;
template auto to_mat( Rotation3d const& translation ) -> glm::mat4;
template auto to_mat( Scale3d const& translation ) -> glm::mat4;

auto normals_transform( glm::mat4 const& world_from_local ) -> glm::mat3
{
    return glm::transpose( glm::inverse( glm::mat3( world_from_local ) ) );
}

} // namespace ltb::math
