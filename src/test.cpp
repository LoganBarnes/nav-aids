#include <type_traits>

using GLenum = unsigned int;

#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31

namespace ltb::ogl
{

struct Program
{
};

template < GLenum shader_type >
struct Shader
{
};

template < template < GLenum... > typename TemplateType, typename Other >
class AreSameEnumTemplate : public std::false_type
{
};

template < template < GLenum... > typename TemplateType, GLenum... types >
class AreSameEnumTemplate< TemplateType, TemplateType< types... > > : public std::true_type
{
};

template < template < GLenum... > typename TemplateType, typename Other >
constexpr auto are_same_enum_template_v = AreSameEnumTemplate< TemplateType, Other >::value;

static_assert( are_same_enum_template_v< Shader, Shader< GL_GEOMETRY_SHADER > > );

template < template < GLenum... > typename TemplateType >
struct GetElement
{
    template < typename Type >
    auto operator( )( Type& el ) const
    {
        if constexpr ( are_same_enum_template_v< TemplateType, decltype( el ) > )
        {
            return std::make_tuple( el );
        }
        else
        {
            return std::make_tuple( );
        }
    }
};

template < template < GLenum... > typename TemplateType >
struct ExtractFromTuple
{
    GetElement< TemplateType > get_element;

    template < typename... Types >
    auto operator( )( Types&... tuple_elements )
    {
        return std::tuple_cat( get_element( tuple_elements )... );
    }
};

template < template < GLenum... > typename TemplateType, typename... Ts >
constexpr auto extract_from_tuple( std::tuple< Ts&... > tuple )
{
    return std::apply( ExtractFromTuple< TemplateType >{ }, std::move( tuple ) );
}

} // namespace ltb::ogl

auto main( ) -> int
{
    using namespace ltb;

    ogl::Shader< GL_VERTEX_SHADER >   vertex_shader( "vertex.glsl" );
    ogl::Shader< GL_FRAGMENT_SHADER > fragment_shader( "fragment.glsl" );

    ogl::Program program{ };

    std::
        tuple< ogl::Shader< GL_VERTEX_SHADER >&, ogl::Shader< GL_FRAGMENT_SHADER >&, ogl::Program& >
            original_tuple = std::tie( vertex_shader, fragment_shader, program );

    std::tuple< ogl::Shader< GL_VERTEX_SHADER >&, ogl::Shader< GL_FRAGMENT_SHADER >& > shaders
        = ogl::extract_from_tuple< ogl::Shader >( original_tuple );

    return 0;
}
