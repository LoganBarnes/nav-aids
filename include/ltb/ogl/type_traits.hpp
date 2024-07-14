#pragma once

// opengl
#include "opengl.hpp"

// standard
#include <concepts>

namespace ltb::ogl
{

template < GLenum T, GLenum... Ts >
concept IsAnyOf = std::bool_constant< ( ( T == Ts ) || ... ) >::value;

// template < GLenum T, GLenum... Ts >
// constexpr auto is_any_v = IsAnyOf< T, Ts... >::value;

template < typename T, typename... Ts >
concept IsAny = ( std::same_as< T, Ts > || ... );

// template < typename T, typename... Ts >
// constexpr auto is_any_same_v = IsAny< T, Ts... >::value;

} // namespace ltb::ogl
