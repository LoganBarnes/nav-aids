#pragma once

// Need to define this before <imgui.h> we can use `glm::vec` with `ImGui`.
#include <glm/glm.hpp>
#define IM_VEC2_CLASS_EXTRA                                                                        \
    constexpr ImVec2( glm::vec2 const& f )                                                         \
        : x( f.x )                                                                                 \
        , y( f.y )                                                                                 \
    {                                                                                              \
    }                                                                                              \
    operator glm::vec2( ) const                                                                    \
    {                                                                                              \
        return glm::vec2( x, y );                                                                  \
    }

#define IM_VEC4_CLASS_EXTRA                                                                        \
    constexpr ImVec4( glm::vec4 const& f )                                                         \
        : x( f.x )                                                                                 \
        , y( f.y )                                                                                 \
        , z( f.z )                                                                                 \
        , w( f.w )                                                                                 \
    {                                                                                              \
    }                                                                                              \
    operator glm::vec4( ) const                                                                    \
    {                                                                                              \
        return glm::vec4( x, y, z, w );                                                            \
    }

// external
#include <imgui.h>
#include <imgui_internal.h>
