#include "ltb/gps/utils.hpp"

// project
#include "ltb/gps/constants.hpp"
#include "ltb/utils/types.hpp"

namespace ltb::gps
{

auto cartesian_from_lat_long_alt_Mm( glm::vec3 const& lat_long_alt_Mm ) -> glm::vec3
{
    auto const lat = glm::radians( lat_long_alt_Mm.x );
    auto const lon = glm::radians( lat_long_alt_Mm.y );

    auto const lat_cos = glm::cos( lat );
    auto const lat_sin = glm::sin( lat );
    auto const lon_cos = glm::cos( lon );
    auto const lon_sin = glm::sin( lon );

    auto const unit_sphere = glm::vec3{
        lat_cos * lon_cos,
        lat_cos * lon_sin,
        lat_sin,
    };

    auto const radius = Constants< float32 >::earth_radius_Mm( ) + lat_long_alt_Mm.z;

    return unit_sphere * radius;
}

} // namespace ltb::gps
