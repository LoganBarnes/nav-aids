#pragma once

namespace ltb::utils
{

struct IsTrue
{
    auto operator( )( bool const value ) const { return value; }
};

struct IsFalse
{
    auto operator( )( bool const value ) const { return !value; }
};

} // namespace ltb::utils
