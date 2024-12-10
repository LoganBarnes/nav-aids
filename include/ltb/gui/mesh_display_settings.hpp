#pragma once

// project
#include "ltb/math/transforms.hpp"

namespace ltb::gui
{

/// \brief The mode used to colorize a mesh.
enum class ColorMode : uint32
{
    White,
    Positions,
    Normals,
    UvCoords,
    VertexColor,
    GlobalColor,
    Texture,
};

/// \brief The mode used to shade a mesh.
enum class ShadingMode : uint32
{
    Flat,
    Lambertian,
    CookTorrance,
};

/// \brief Settings used to color and display lidar points.
struct MeshDisplaySettings
{
    bool        visible      = true;
    ColorMode   color_mode   = ColorMode::White;
    glm::vec4   custom_color = glm::vec4( 1.0F );
    ShadingMode shading_mode = ShadingMode::Flat;

    /// \brief The transforms to apply to the mesh.
    std::vector< math::Transform3d > transforms = { };
};

} // namespace ltb::gui
