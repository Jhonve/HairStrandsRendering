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
  
    void draw(int index_count, bool is_shade = true) override;

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
  
    void draw(int index_count, bool is_shade = true) override;

};

class OpenGLQuadIndexBuffer : public VertexIndexBuffer
{
public:
    OpenGLQuadIndexBuffer() : VertexIndexBuffer()
    {}
  
    void create_buffers(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices) override;
  
    void delete_buffers() override;
  
    void bind() override;
  
    void unbind() override;
  
    void draw(int index_count, bool is_shade = true) override;

};
class OpenGLFrameBuffer : public FrameBuffer
{
public:

    void create_buffers(int width, int height) override;

    void delete_buffers() override;

    void bind() override;

    void unbind() override;
  
    uint32_t get_texture() override;
};

class OpenGLFrameBuffers
{
public:
    OpenGLFrameBuffers();
    ~OpenGLFrameBuffers();
    
    bool create_textures();
    bool create_FBO();
    bool attach_FBO_textures();

    bool create_buffers(int width, int height);

    GLFBO& get_comp_FBO() { return m_comp_FBO; }
    GLFBO& get_mesh_FBO() { return m_mesh_FBO; }
    GLFBO& get_strands_FBO(){ return m_strands_FBO; }
    GLFBO& get_transparency_slab_FBO(){ return m_trsp_slab_FBO; }
    GLFBO& get_transparency_occupancy_FBO(){ return m_trsp_occ_FBO; }
    GLFBO& get_transparency_depth_range_FBO(){ return m_trsp_depth_range_FBO;}
    GLFBO& get_shadow_opacity_FBO(){ return m_shadow_opacity_FBO;}
    GLFBO& get_shadow_depth_FBO(){ return m_shadow_depth_FBO; }

protected:
    GLFBO m_comp_FBO;
    GLTexture m_comp_color_tex;
    GLTexture m_comp_depth_tex;

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

    int m_frame_width = 0;
    int m_frame_height = 0;
    int m_shadow_width = 0;
    int m_shadow_height = 0;
};