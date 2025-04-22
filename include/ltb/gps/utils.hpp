#pragma once

// external
#include <glm/glm.hpp>

namespace ltb::gps
{

auto cartesian_from_lat_long_alt_Mm( glm::vec3 const& lat_long_alt_Mm ) -> glm::vec3;

} // namespace ltb::gps
