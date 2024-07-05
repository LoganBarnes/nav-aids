#include "ltb/ogl/uniform_setter.hpp"

// project
#include "ltb/ogl/program.hpp"

namespace ltb::ogl
{

template < typename ValueType >
auto UniformSetter< ValueType >::operator( )(
    Program const&     program,
    std::string const& name,
    ValueType const&   value
) const -> utils::Result< Program const* >
{
    return program.set_scalar_or_vec_uniform< ValueType >( name, &value, 1, 1 );
}

template struct UniformSetter< bool >;
template struct UniformSetter< int32 >;
template struct UniformSetter< uint32 >;
template struct UniformSetter< float32 >;
template struct UniformSetter< float64 >;

} // namespace ltb::ogl
