#pragma once

// project
#include "ltb/math/mesh.hpp"
#include "ltb/utils/types.hpp"

namespace ltb::math::shapes
{

struct Icosphere
{
    glm::vec3 position        = { 0.0F, 0.0F, 0.0F };
    float32   radius          = 1.0F;
    uint32    recursion_level = 2U;
};

auto build_mesh( Icosphere const& icosphere ) -> math::Mesh3;

} // namespace ltb::math::shapes
