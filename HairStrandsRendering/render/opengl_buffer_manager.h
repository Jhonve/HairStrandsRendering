#pragma once
#include "render_base.h"
#include <vector>

class OpenGLVertexIndexBuffer : public VertexIndexBuffer
{
public:
    OpenGLVertexIndexBuffer() : VertexIndexBuffer()
    {}
  
    void create_buffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) override;
  
    void delete_buffers() override;
  
    void bind() override;
  
    void unbind() override;
  
    void draw(int index_count) override;

};

class OpenGLFrameBuffer : public FrameBuffer
{
public:

    void create_buffers(int32_t width, int32_t height) override;

    void delete_buffers() override;

    void bind() override;

    void unbind() override;
  
    uint32_t get_texture() override;
};