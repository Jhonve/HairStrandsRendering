#pragma once
#include "render/render_base.h"
#include <vector>

class OpenGLVertexIndexBuffer : public VertexIndexBuffer
{
public:
    OpenGLVertexIndexBuffer() : VertexIndexBuffer()
    {}
  
    void create_buffers(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices) override;
  
    void delete_buffers() override;
  
    void bind() override;
  
    void unbind() override;
  
    void draw(int index_count) override;

};

class OpenGLStrandsIndexBuffer : public StrandsIndexBuffer
{
public:
    OpenGLStrandsIndexBuffer() : StrandsIndexBuffer()
    {}
  
    void create_buffers(const std::vector<StrandVertex>& vertices,
        const std::vector<unsigned int>& indices) override;
  
    void delete_buffers() override;
  
    void bind() override;
  
    void unbind() override;
  
    void draw(int index_count) override;

};

class OpenGLFrameBuffer : public FrameBuffer
{
public:

    void create_buffers(int32_t width, int32_t height) override;
    void create_rgbaui_buffer(int32_t width, int32_t height);
    void create_rgbaf_buffer(int32_t width, int32_t height);
    void create_depth_buffer(int32_t width, int32_t height);

    void delete_buffers() override;

    void bind() override;

    void unbind() override;
  
    uint32_t get_texture() override;

protected:
    uint32_t m_depth_tex_id = 0;
};