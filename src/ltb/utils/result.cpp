#include "ltb/utils/result.hpp"

namespace ltb::utils
{

auto success( ) -> Result< void >
{
    return { };
}

} // namespace ltb::utils
