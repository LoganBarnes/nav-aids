// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "result.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>

namespace ltb::utils
{

template < typename T >
auto get_binary_file_contents( std::filesystem::path const& file_path ) -> utils::Result< std::vector< T > >
{
    auto file = std::ifstream( file_path, std::ios::ate | std::ios::binary );

    if ( !file.is_open( ) )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Failed to open file '{}'", file_path.string( ) );
    }

    auto file_size = static_cast< size_t >( file.tellg( ) );

    auto buffer_size = file_size / sizeof( T );
    auto remainder   = file_size % sizeof( T );

    auto buffer = std::vector< T >( buffer_size + std::clamp< decltype( remainder ) >( remainder, 0, 1 ) );

    file.seekg( 0 );
    file.read( reinterpret_cast< char* >( buffer.data( ) ), static_cast< std::streamsize >( file_size ) );
    file.close( );

    return buffer;
}

template < typename T >
auto get_binary_files_contents( std::vector< std::filesystem::path > const& file_paths )
    -> utils::Result< std::vector< std::vector< T > > >
{

    auto buffers = std::vector< std::vector< T > >{ };
    buffers.reserve( file_paths.size( ) );

    for ( auto const& file_path : file_paths )
    {
        auto buffer = get_binary_file_contents< T >( file_path );
        if ( !buffer )
        {
            return tl::make_unexpected( buffer.error( ) );
        }
        buffers.emplace_back( std::move( buffer.value( ) ) );
    }

    return buffers;
}

/// \brief Get the binary contents of a file.
// auto get_binary_file_contents(std::filesystem::path const& file_path) -> utils::Result<std::vector<char>>;

} // namespace ltb::utils
