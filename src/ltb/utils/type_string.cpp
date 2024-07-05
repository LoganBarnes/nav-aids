#include "ltb/utils/type_string.hpp"

// project
#include "ltb/utils/types.hpp"

// standard
#include <regex>

#if defined( __GNUG__ )

// platform
#include <cstdlib>
#include <cxxabi.h>
#include <memory>

namespace ltb::utils::detail
{

auto demangle( char const* const name ) -> std::string
{
    auto       status = int32{ 0 };
    auto const str    = std::unique_ptr< char >(
        // Get the string name without the extra characters
        abi::__cxa_demangle( name, nullptr, nullptr, &status )
    );

    if ( 0 == status )
    {
        return str.get( );
    }
    else
    {
        return name;
    }
}

#else

namespace ltb::utils::detail
{

// does nothing if not g++
std::string demangle( char const* name )
{
    return name;
}

#endif

namespace
{

constexpr char const* ugly_string1
    = "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >";

constexpr char const* ugly_string2
    = "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >";

} // namespace

auto replace_ugly_strings( std::string type_str ) -> std::string
{
    type_str
        = std::regex_replace( type_str, std::regex( ugly_string1 ), std::string( "std::string>" ) );
    type_str
        = std::regex_replace( type_str, std::regex( ugly_string2 ), std::string( "std::string" ) );
    return type_str;
}

} // namespace ltb::utils
