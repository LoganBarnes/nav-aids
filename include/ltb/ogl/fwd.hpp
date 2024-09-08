#pragma once

// graphics
#include "ltb/ogl/opengl.hpp"

namespace ltb::ogl
{

class Buffer;
struct BufferData;

class Framebuffer;
struct FramebufferData;

class Program;
struct ProgramData;

struct ShaderData;

class Texture;
struct TextureData;

template < typename ValueType >
class Uniform;

class VertexArray;
struct VertexArrayData;

template < typename OglObj, GLenum bind_type = GL_INVALID_ENUM >
class Bound;

} // namespace ltb::ogl
