#pragma once

namespace ltb::ogl
{

class Buffer;
struct BufferData;

class Framebuffer;
struct FramebufferData;

class Program;
struct ProgramData;

struct ShaderData;

class ShaderProgram;
struct ShaderProgramData;

class Texture;
struct TextureData;

template < typename ValueType, GLenum bind_type = GL_INVALID_ENUM >
class Uniform;

class VertexArray;
struct VertexArrayData;

} // namespace ltb::ogl
