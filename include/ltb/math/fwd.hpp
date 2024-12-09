// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/utils/types.hpp"

// external
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ltb::math
{

struct ModelUniforms;

template < glm::length_t N, typename T = float32 >
struct Mesh;

using Mesh3 = Mesh< 3 >;
using Mesh2 = Mesh< 2 >;

template < typename T >
struct Range;

using Range2Di = Range< glm::ivec2 >;
using Range2D  = Range< glm::vec2 >;

} // namespace ltb::math
