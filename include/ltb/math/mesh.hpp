
// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/math/fwd.hpp"

// external
#include <glm/glm.hpp>

// standard
#include <vector>

namespace ltb::math
{

enum class MeshFormat
{
    Points,
    Lines,
    LineStrip,
    Triangles,
    TriangleStrip,
};

template < glm::length_t N, typename T >
struct Mesh
{
    MeshFormat format = MeshFormat::Triangles;

    std::vector< glm::vec< N, T > >     positions     = { };
    std::vector< glm::vec< N, T > >     normals       = { };
    std::vector< glm::vec< N - 1, T > > uvs           = { };
    std::vector< glm::vec3 >            vertex_colors = { };

    std::vector< uint32 > indices = { };
};

} // namespace ltb::math
