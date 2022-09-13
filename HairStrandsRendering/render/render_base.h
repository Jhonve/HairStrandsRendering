#pragma once

#include "window.h"
#include "elems/vertex.h"
#include "elems/strand.h"
#include "glad/glad.h"

class VertexIndexBuffer
{
public:
    VertexIndexBuffer() : m_VBO{ 0 }, m_VAO{ 0 }, m_IBO{ 0 }
    {}
  
    virtual void create_buffers(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices) = 0;
  
    virtual void delete_buffers() = 0;
  
    virtual void bind() = 0;
  
    virtual void unbind() = 0;
  
    virtual void draw(int index_count) = 0;

protected:
    GLuint m_VBO;
    GLuint m_VAO;
    GLuint m_IBO;
};

class StrandsIndexBuffer
{
public:
    StrandsIndexBuffer() : m_VBO{ 0 }, m_VAO{ 0 }, m_IBO{ 0 }
    {}
  
    virtual void create_buffers(const std::vector<Strand>& strands,
        const std::vector<std::vector<unsigned int>>& indices) = 0;
  
    virtual void delete_buffers() = 0;
  
    virtual void bind() = 0;
  
    virtual void unbind() = 0;
  
    virtual void draw(int index_count) = 0;

protected:
    GLuint m_VBO;
    GLuint m_VAO;
    GLuint m_IBO;
};

class FrameBuffer
{
public:
    FrameBuffer() : m_FBO { 0 }, m_depth_id {0}
    {}
  
    virtual void create_buffers(int32_t width, int32_t height) = 0;
  
    virtual void delete_buffers() = 0;
  
    virtual void bind() = 0;
  
    virtual void unbind() = 0;
  
    virtual uint32_t get_texture() = 0;

protected:
    uint32_t m_FBO = 0;
    uint32_t m_tex_id = 0;
    uint32_t m_depth_id = 0;
    int32_t m_width = 0;
    int32_t m_height = 0;
};

class RenderContext
{

public:
    RenderContext() : m_window(nullptr) {}

    virtual bool init(ImWindow* window)
    {
        m_window = window;
        return true;
    }

    virtual void pre_render() = 0;

    virtual void post_render() = 0;

    virtual void end() = 0;

protected:
    ImWindow* m_window;
};