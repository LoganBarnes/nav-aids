#pragma once

// project
#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/program.hpp"
#include "ltb/ogl/program_attribute.hpp"
#include "ltb/ogl/program_uniform.hpp"
#include "ltb/ogl/shader.hpp"
#include "ltb/ogl/type_traits.hpp"
#include "ltb/ogl/vertex_array.hpp"

// external
#include <pfr.hpp>
#include <spdlog/spdlog.h>

// standard
#include <filesystem>
#include <tuple>
#include <utility>

namespace ltb::ogl
{

template < typename T >
concept Initializable = requires( T a ) {
    { a.initialize( ) } -> std::same_as< utils::Result<> >;
};

template < template < typename... > typename TemplateType, typename Other >
class AreSameTemplate : public std::false_type
{
};

template < template < typename... > typename TemplateType, typename... Types >
class AreSameTemplate< TemplateType, TemplateType< Types... >& > : public std::true_type
{
};

template < template < GLenum... > typename TemplateType, typename Other >
class AreSameEnumTemplate : public std::false_type
{
};

template < template < GLenum... > typename TemplateType, GLenum... types >
class AreSameEnumTemplate< TemplateType, TemplateType< types... >& > : public std::true_type
{
};

template < template < typename... > typename TemplateType, typename Other >
constexpr auto are_same_template_v = AreSameTemplate< TemplateType, Other >::value;

template < template < GLenum... > typename TemplateType, typename Other >
constexpr auto are_same_enum_template_v = AreSameEnumTemplate< TemplateType, Other >::value;

template < template < typename... > typename TemplateType >
struct GetElement
{
    template < typename Type >
    auto operator( )( Type& el ) const
    {
        if constexpr ( are_same_template_v< TemplateType, decltype( el ) > )
        {
            return std::tuple< Type& >( el );
        }
        else
        {
            return std::make_tuple( );
        }
    }
};

template < template < GLenum... > typename TemplateType >
struct GetEnumElement
{
    template < typename Type >
    auto operator( )( Type& el ) const
    {
        if constexpr ( are_same_enum_template_v< TemplateType, decltype( el ) > )
        {
            return std::tuple< Type& >( el );
        }
        else
        {
            return std::make_tuple( );
        }
    }
};

template < typename Predicate >
struct ExtractFromTuple
{
    Predicate predicate;

    template < typename... Types >
    auto operator( )( Types&... tuple_elements )
    {
        return std::tuple_cat( predicate( tuple_elements )... );
    }
};

template < template < typename... > typename TemplateType, typename... Ts >
constexpr auto extract_from_tuple( std::tuple< Ts&... > tuple )
{
    using Functor = ExtractFromTuple< GetElement< TemplateType > >;
    return std::apply( Functor{ }, std::move( tuple ) );
}

template < template < GLenum... > typename TemplateType, typename... Ts >
constexpr auto extract_from_tuple( std::tuple< Ts&... > tuple )
{
    using Functor = ExtractFromTuple< GetEnumElement< TemplateType > >;
    return std::apply( Functor{ }, std::move( tuple ) );
}

struct FailedResult
{
    utils::Result<> result;

    template < typename Type >
        requires Initializable< Type >
    auto operator( )( Type& object ) -> bool
    {
        result = object.initialize( );
        return !result;
    }
};

template < typename... Objs, std::size_t... Is >
auto initialize( std::tuple< Objs&... >& tuple, std::index_sequence< Is... > ) -> utils::Result<>
{
    if ( auto failed_result = FailedResult{ }; ( failed_result( std::get< Is >( tuple ) ) || ... ) )
    {
        return failed_result.result;
    }
    return utils::success( );
}

template < typename... Objs >
auto initialize( std::tuple< Objs&... >& tuple ) -> utils::Result<>
{
    return initialize( tuple, std::index_sequence_for< Objs... >( ) );
}

template < GLenum... shader_types, std::size_t... Is >
auto initialize(
    Program&                                  program,
    std::tuple< Shader< shader_types >&... >& shaders,
    std::index_sequence< Is... >              indices
) -> utils::Result<>
{
    utils::ignore( indices );

    LTB_CHECK( initialize( shaders ) );
    LTB_CHECK( program.initialize( ) );
    LTB_CHECK( program.attach_and_link( std::get< Is >( shaders )... ) );

    return utils::success( );
}

template < GLenum... shader_types >
auto initialize( Program& program, std::tuple< Shader< shader_types >&... >& shaders )
    -> utils::Result<>
{
    return initialize( program, shaders, std::index_sequence_for< Shader< shader_types >... >( ) );
}

template < typename Pipeline >
auto initialize_pipeline( Program& program, Pipeline& pipeline ) -> utils::Result<>
{
    auto const& tuple = pfr::structure_tie( pipeline );

    auto shaders    = extract_from_tuple< Shader >( tuple );
    auto attributes = extract_from_tuple< Attribute >( tuple );
    auto uniforms   = extract_from_tuple< Uniform >( tuple );

    LTB_CHECK( initialize( program, shaders ) );
    LTB_CHECK( initialize( attributes ) );
    LTB_CHECK( initialize( uniforms ) );

    return utils::success( );
}

} // namespace ltb::ogl
