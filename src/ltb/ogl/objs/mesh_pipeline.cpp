#include "ltb/ogl/objs/mesh_pipeline.hpp"

// project
#include "ltb/ogl/object_id.hpp"

namespace ltb::ogl::objs
{

auto MeshPipeline::initialize( ) -> utils::Result<>
{
    if ( this->is_initialized( ) )
    {
        return utils::success( );
    }

    return utils::success( );
}

auto MeshPipeline::is_initialized( ) const -> bool
{
    return false;
}

} // namespace ltb::ogl::objs
