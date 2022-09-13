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

    void delete_buffers() override;

    void bind() override;

    void unbind() override;
  
    uint32_t get_texture() override;
};

class OpenGLFrameBuffers : public FrameBuffers
{
public:

    void create_buffers(int32_t width, int32_t height) override;

    void delete_buffers() override;

    void bind() override;

    void unbind() override;
  
    uint32_t get_texture() override;

protected:
    GLFBO m_mesh_FBO;
    GLTexture m_mesh_color_tex;
    GLTexture m_mesh_depth_tex;

    GLFBO m_strands_FBO;
    GLTexture m_strands_color_tex;
    GLTexture m_strands_depth_tex;

    GLFBO m_trsp_slab_FBO;
    GLTexture m_trsp_slab_color_tex;

    GLFBO m_trsp_occ_FBO;
    GLTexture m_trsp_occ_color_tex;

    GLFBO m_trsp_depth_range_FBO;
    GLTexture m_trsp_depth_range_color_tex;

    GLFBO m_shadow_opacity_FBO;
    GLTexture m_shadow_opacity_color_tex;

    GLFBO m_shadow_depth_FBO;
    GLTexture m_shadow_depth_color_tex;
    GLTexture m_shadow_depth_depth_tex;

    int m_frame_width;
    int m_frame_height;
    int m_shadow_width;
    int m_shadow_height;
};