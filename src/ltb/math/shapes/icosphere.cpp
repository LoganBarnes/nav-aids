#include "ltb/math/shapes/icosphere.hpp"

// standard
#include <map>
#include <ranges>

namespace ltb::math::shapes
{
namespace
{

using IndexType        = uint32;
using OuterIndices     = std::pair< IndexType, IndexType >;
using MiddlePointCache = std::map< OuterIndices, uint32 >;

auto get_middle_point(
    Mesh3&            mesh,
    uint64 const      index1,
    uint64 const      index2,
    MiddlePointCache& cache
)
{
    // first check if we have it already
    auto const key = std::make_pair( std::min( index1, index2 ), std::max( index1, index2 ) );
    if ( auto iter = cache.find( key ); iter != cache.end( ) )
    {
        return iter->second;
    }

    // not in cache, calculate it
    auto const point1 = mesh.positions[ index1 ];
    auto const point2 = mesh.positions[ index2 ];
    auto const middle = glm::mix( point1, point2, 0.5F );

    auto const index = static_cast< uint32 >( mesh.positions.size( ) );

    // add vertex makes sure point is on unit sphere
    mesh.positions.emplace_back( glm::normalize( middle ) );

    // store it, return index
    cache.try_emplace( key, index );

    return index;
}

auto refine_unit_icosphere( Mesh3& mesh, uint32 const recursion_level )
{
    auto cache = MiddlePointCache{ };

    // refine triangles
    for ( auto ri = 0U; ri < recursion_level; ++ri )
    {
        auto refined_indices = std::vector< uint32 >{ };
        for ( auto i = 0U; i < mesh.indices.size( ); i += 3U )
        {
            auto const i0 = mesh.indices[ i + 0U ];
            auto const i1 = mesh.indices[ i + 1U ];
            auto const i2 = mesh.indices[ i + 2U ];

            // replace triangle by 4 triangles
            auto const a = get_middle_point( mesh, i0, i1, cache );
            auto const b = get_middle_point( mesh, i1, i2, cache );
            auto const c = get_middle_point( mesh, i2, i0, cache );

            refined_indices.insert( refined_indices.end( ), { i0, a, c } );
            refined_indices.insert( refined_indices.end( ), { i1, b, a } );
            refined_indices.insert( refined_indices.end( ), { i2, c, b } );
            refined_indices.insert( refined_indices.end( ), { a, b, c } );
        }
        mesh.indices = std::move( refined_indices );
    }
}

auto build_base_unit_icosphere( )
{
    auto mesh = Mesh3{ MeshFormat::Triangles };

    // create 12 vertices of an icosahedron
    auto const t = ( 1.0F + std::sqrt( 5.0F ) ) / 2.0F;

    mesh.positions.emplace_back( glm::normalize( glm::vec3( -1.0F, +t, 0.0F ) ) );
    mesh.positions.emplace_back( glm::normalize( glm::vec3( +1.0F, +t, 0.0F ) ) );
    mesh.positions.emplace_back( glm::normalize( glm::vec3( -1.0F, -t, 0.0F ) ) );
    mesh.positions.emplace_back( glm::normalize( glm::vec3( +1.0F, -t, 0.0F ) ) );

    mesh.positions.emplace_back( glm::normalize( glm::vec3( 0.0F, -1.0F, +t ) ) );
    mesh.positions.emplace_back( glm::normalize( glm::vec3( 0.0F, +1.0F, +t ) ) );
    mesh.positions.emplace_back( glm::normalize( glm::vec3( 0.0F, -1.0F, -t ) ) );
    mesh.positions.emplace_back( glm::normalize( glm::vec3( 0.0F, +1.0F, -t ) ) );

    mesh.positions.emplace_back( glm::normalize( glm::vec3( +t, 0.0F, -1.0F ) ) );
    mesh.positions.emplace_back( glm::normalize( glm::vec3( +t, 0.0F, +1.0F ) ) );
    mesh.positions.emplace_back( glm::normalize( glm::vec3( -t, 0.0F, -1.0F ) ) );
    mesh.positions.emplace_back( glm::normalize( glm::vec3( -t, 0.0F, +1.0F ) ) );

    // create 20 triangles of the icosahedron

    // 5 faces around point 0
    mesh.indices.insert( mesh.indices.end( ), { 0U, 11U, 5U } );
    mesh.indices.insert( mesh.indices.end( ), { 0U, 5U, 1U } );
    mesh.indices.insert( mesh.indices.end( ), { 0U, 1U, 7U } );
    mesh.indices.insert( mesh.indices.end( ), { 0U, 7U, 10U } );
    mesh.indices.insert( mesh.indices.end( ), { 0U, 10U, 11U } );

    // 5 adjacent faces
    mesh.indices.insert( mesh.indices.end( ), { 1U, 5U, 9U } );
    mesh.indices.insert( mesh.indices.end( ), { 5U, 11U, 4U } );
    mesh.indices.insert( mesh.indices.end( ), { 11U, 10U, 2U } );
    mesh.indices.insert( mesh.indices.end( ), { 10U, 7U, 6U } );
    mesh.indices.insert( mesh.indices.end( ), { 7U, 1U, 8U } );

    // 5 faces around point 3
    mesh.indices.insert( mesh.indices.end( ), { 3U, 9U, 4U } );
    mesh.indices.insert( mesh.indices.end( ), { 3U, 4U, 2U } );
    mesh.indices.insert( mesh.indices.end( ), { 3U, 2U, 6U } );
    mesh.indices.insert( mesh.indices.end( ), { 3U, 6U, 8U } );
    mesh.indices.insert( mesh.indices.end( ), { 3U, 8U, 9U } );

    // 5 adjacent faces
    mesh.indices.insert( mesh.indices.end( ), { 4U, 9U, 5U } );
    mesh.indices.insert( mesh.indices.end( ), { 2U, 4U, 11U } );
    mesh.indices.insert( mesh.indices.end( ), { 6U, 2U, 10U } );
    mesh.indices.insert( mesh.indices.end( ), { 8U, 6U, 7U } );
    mesh.indices.insert( mesh.indices.end( ), { 9U, 8U, 1U } );

    return mesh;
}

struct TransformUnitPoints
{
    Icosphere const& icosphere;

    auto operator( )( glm::vec3 const& position ) const
    {
        return icosphere.position + ( icosphere.radius * position );
    }
};

} // namespace

auto build_mesh( Icosphere const& icosphere ) -> Mesh3
{
    auto mesh = build_base_unit_icosphere( );
    refine_unit_icosphere( mesh, icosphere.recursion_level );

    mesh.normals = mesh.positions;
    std::transform(
        mesh.normals.begin( ),
        mesh.normals.end( ),
        mesh.positions.begin( ),
        TransformUnitPoints{ icosphere }
    );

    return mesh;
}

} // namespace ltb::math::shapes
