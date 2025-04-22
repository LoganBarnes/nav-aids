#include "ltb/gps/utils.hpp"

// project
#include "ltb/gps/constants.hpp"
#include "ltb/utils/types.hpp"

// external
#include <glm/gtc/matrix_transform.hpp>

namespace ltb::gps
{

auto cartesian_from_lat_long_alt_Mm( glm::vec3 const& lat_long_alt_Mm ) -> glm::vec3
{
    auto const unit_sphere = glm::rotate(
                                 glm::identity< glm::mat4 >( ),
                                 glm::radians( lat_long_alt_Mm.y ),
                                 glm::vec3{ 0.0F, 1.0F, 0.0F }
                             )
                           * glm::rotate(
                                 glm::identity< glm::mat4 >( ),
                                 glm::radians( lat_long_alt_Mm.x ),
                                 glm::vec3{ 0.0F, 0.0F, 1.0F }
                           )
                           * glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );

    auto const radius = Constants< float32 >::earth_radius_Mm( ) + lat_long_alt_Mm.z;

    return glm::vec3( unit_sphere ) * radius;
}

} // namespace ltb::gps
