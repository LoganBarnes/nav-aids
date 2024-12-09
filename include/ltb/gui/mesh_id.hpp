#pragma once

// project
#include "ltb/utils/types.hpp"

// standard
#include <compare>
#include <string> // std::hash

namespace ltb::gui
{

/// \brief A unique identifier for an allocated piece of geometry.
class MeshId
{
public:
    constexpr MeshId( ) = default;

    explicit constexpr MeshId( uint32 const id )
        : id_( id )
    {
    }

    [[nodiscard( "Const Getter" )]] auto id( ) const -> uint32;

    explicit operator bool( ) const;

    auto operator<=>( MeshId const& ) const = default;

private:
    uint32 id_ = 0U;
};

} // namespace ltb::gui

namespace std
{

template <>
struct hash< ltb::gui::MeshId >
{
    auto operator( )( ltb::gui::MeshId const& id ) const -> size_t
    {
        return std::hash< ltb::uint32 >{ }( id.id( ) );
    }
};

} // namespace std
