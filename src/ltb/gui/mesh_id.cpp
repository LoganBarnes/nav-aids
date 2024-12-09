#include "ltb/gui/mesh_id.hpp"

namespace ltb::gui
{

auto MeshId::id( ) const -> uint32_t
{
    return id_;
}

MeshId::operator bool( ) const
{
    return id_ != 0U;
}

} // namespace ltb::gui
