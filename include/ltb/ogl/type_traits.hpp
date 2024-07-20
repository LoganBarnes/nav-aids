#pragma once

// opengl
#include "opengl.hpp"

// standard
#include <concepts>

namespace ltb::ogl
{

template < GLenum T, GLenum... Ts >
concept IsAnyOf = std::bool_constant< ( ( T == Ts ) || ... ) >::value;

template < typename T, typename... Ts >
concept IsAny = ( std::same_as< T, Ts > || ... );

} // namespace ltb::ogl
