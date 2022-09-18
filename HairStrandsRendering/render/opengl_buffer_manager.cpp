#include "render/opengl_buffer_manager.h"
#include "glad/glad.h"

/*
* Vertex buffer class for mesh vertex
*/

void OpenGLVertexIndexBuffer::create_buffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    glGenVertexArrays(1, &m_VAO);
  
    glGenBuffers(1, &m_IBO);
    glGenBuffers(1, &m_VBO);
  
    bind();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    unbind();
}

void OpenGLVertexIndexBuffer::delete_buffers()
{
    glDeleteBuffers(1, &m_IBO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void OpenGLVertexIndexBuffer::bind()
{
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
}

void OpenGLVertexIndexBuffer::unbind()
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLVertexIndexBuffer::draw(int index_count, bool is_shade)
{
    bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_pos));
    
    if (is_shade)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
    }

    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    
    glDisableVertexAttribArray(0);
    if(is_shade)
        glDisableVertexAttribArray(1);

    unbind();
}

/*
* Vertex buffer class for strands vertex
*/

void OpenGLStrandsIndexBuffer::create_buffers(const std::vector<StrandVertex>& vertices,
        const std::vector<unsigned int>& indices)
{
    glGenVertexArrays(1, &m_VAO);
 
    glGenBuffers(1, &m_IBO);
    glGenBuffers(1, &m_VBO);

    bind();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StrandVertex), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    unbind();
}

void OpenGLStrandsIndexBuffer::delete_buffers()
{
    glDeleteBuffers(1, &m_IBO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void OpenGLStrandsIndexBuffer::bind()
{
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
}

void OpenGLStrandsIndexBuffer::unbind()
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLStrandsIndexBuffer::draw(int index_count, bool is_shade)
{
    bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StrandVertex), (void*)offsetof(StrandVertex, m_pos));
    
    if (is_shade)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(StrandVertex), (void*)offsetof(StrandVertex, m_tangent));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(StrandVertex), (void*)offsetof(StrandVertex, m_color));
    }

    // the vertices as line loop
    glLineWidth(3);
    glDrawElements(GL_LINES, index_count, GL_UNSIGNED_INT, nullptr);

    glDisableVertexAttribArray(0);
    if(is_shade)
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

    unbind();
}


/*
* Vertex buffer class for mesh vertex
*/

void OpenGLQuadIndexBuffer::create_buffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    glGenVertexArrays(1, &m_VAO);
  
    glGenBuffers(1, &m_IBO);
    glGenBuffers(1, &m_VBO);
  
    bind();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    unbind();
}

void OpenGLQuadIndexBuffer::delete_buffers()
{
    glDeleteBuffers(1, &m_IBO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void OpenGLQuadIndexBuffer::bind()
{
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
}

void OpenGLQuadIndexBuffer::unbind()
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLQuadIndexBuffer::draw(int index_count, bool is_shade)
{
    bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_pos));
  
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_uv));
    
    glDrawElements(GL_TRIANGLE_FAN, index_count, GL_UNSIGNED_INT, nullptr); // GL_QUADS will be deprecated and not work on macos
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    unbind();
}


/*
* Fragment buffer class for simple mesh vertex
*/

void OpenGLFrameBuffer::create_buffers(int width, int height)
{
    m_width = width;
    m_height = height;

    if (m_FBO)
        delete_buffers();

    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    
    // rgb texture
    glCreateTextures(GL_TEXTURE_2D, 1, &m_color_tex_id);
    glBindTexture(GL_TEXTURE_2D, m_color_tex_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_tex_id, 0);
    
    // depth texture
    glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_tex_id);
    glBindTexture(GL_TEXTURE_2D, m_depth_tex_id);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_width, m_height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth_tex_id, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("ERROR::BUFFERMANAGER:: Framebuffer is not complete for depth texture!\n");
    }

    GLenum buffers[4] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(m_color_tex_id, buffers);

    unbind();
}

void OpenGLFrameBuffer::delete_buffers()
{
    if (m_FBO)
    {
        glDeleteFramebuffers(GL_FRAMEBUFFER, &m_FBO);
        glDeleteTextures(1, &m_color_tex_id);
        glDeleteTextures(1, &m_depth_tex_id);
        m_color_tex_id = 0;
        m_depth_tex_id = 0;
    }

}

void OpenGLFrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLFrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t OpenGLFrameBuffer::get_texture()
{
    return m_color_tex_id;
}

/*
* Fragment buffers class
*/
OpenGLFrameBuffers::OpenGLFrameBuffers()
{
}

OpenGLFrameBuffers::~OpenGLFrameBuffers()
{
    m_mesh_FBO.delete_FBO();
    m_mesh_color_tex.delete_texture();
    m_mesh_depth_tex.delete_texture();

    m_strands_FBO.delete_FBO();
    m_strands_color_tex.delete_texture();
    m_strands_depth_tex.delete_texture();
    
    m_trsp_slab_FBO.delete_FBO();
    m_trsp_slab_color_tex.delete_texture();

    m_trsp_occ_FBO.delete_FBO();
    m_trsp_occ_color_tex.delete_texture();

    m_trsp_depth_range_FBO.delete_FBO();
    m_trsp_depth_range_color_tex.delete_texture();

    m_shadow_opacity_FBO.delete_FBO();
    m_shadow_opacity_color_tex.delete_texture();
    
    m_shadow_depth_FBO.delete_FBO();
    m_shadow_depth_color_tex.delete_texture();
    m_shadow_depth_depth_tex.delete_texture();
}

bool OpenGLFrameBuffers::create_textures()
{
    bool create_sucess = 
        m_comp_color_tex.create_RGBA32F_texture(m_frame_width, m_frame_height, GL_LINEAR) &&
        m_comp_depth_tex.create_depth_texture(m_frame_width, m_frame_height, GL_LINEAR) &&
        m_mesh_color_tex.create_RGBA32F_texture(m_frame_width, m_frame_height, GL_LINEAR) &&
        m_mesh_depth_tex.create_depth_texture(m_frame_width, m_frame_height, GL_LINEAR) &&
        m_strands_color_tex.create_RGBA32F_texture(m_frame_width, m_frame_height, GL_LINEAR) &&
        m_strands_depth_tex.create_depth_texture(m_frame_width, m_frame_height, GL_LINEAR) &&
        m_trsp_slab_color_tex.create_RGBA32F_texture(m_frame_width, m_frame_height, GL_LINEAR) &&
        m_trsp_occ_color_tex.create_RGBA32UI_texture(m_frame_width, m_frame_height, GL_LINEAR) &&
        m_trsp_depth_range_color_tex.create_RGBA32F_texture(m_frame_width, m_frame_height, GL_LINEAR) &&
        m_shadow_opacity_color_tex.create_RGBA32F_texture(m_shadow_width, m_shadow_height, GL_LINEAR) &&
        m_shadow_depth_color_tex.create_RGBA32F_texture(m_shadow_width, m_shadow_height, GL_LINEAR) &&
        m_shadow_depth_depth_tex.create_depth_texture(m_shadow_width, m_shadow_height, GL_LINEAR);

    if (!create_sucess)
        printf("ERROR::BUFFERMANAGER:: create textures wrong!\n");
    return create_sucess;
}

bool OpenGLFrameBuffers::create_FBO()
{
    m_comp_FBO.create_FBO();
    m_mesh_FBO.create_FBO();
    m_strands_FBO.create_FBO();
    m_trsp_slab_FBO.create_FBO();
    m_trsp_occ_FBO.create_FBO();
    m_trsp_depth_range_FBO.create_FBO();
    m_shadow_opacity_FBO.create_FBO();
    m_shadow_depth_FBO.create_FBO();
    return true;
}

bool OpenGLFrameBuffers::attach_FBO_textures()
{
    m_comp_FBO.attach_color_texture(m_comp_color_tex);
    m_comp_FBO.attach_depth_texture(m_comp_depth_tex);
    m_mesh_FBO.attach_color_texture(m_mesh_color_tex);
    m_mesh_FBO.attach_depth_texture(m_mesh_depth_tex);
    m_strands_FBO.attach_color_texture(m_strands_color_tex);
    m_strands_FBO.attach_depth_texture(m_strands_depth_tex);
    m_trsp_slab_FBO.attach_color_texture(m_trsp_slab_color_tex);
    m_trsp_slab_FBO.attach_depth_texture(m_strands_depth_tex);
    m_trsp_occ_FBO.attach_color_texture(m_trsp_occ_color_tex);
    m_trsp_occ_FBO.attach_depth_texture(m_strands_depth_tex);
    m_trsp_depth_range_FBO.attach_color_texture(m_trsp_depth_range_color_tex);
    m_trsp_depth_range_FBO.attach_depth_texture(m_strands_depth_tex);
    m_shadow_depth_FBO.attach_color_texture(m_shadow_depth_color_tex);
    m_shadow_depth_FBO.attach_depth_texture(m_shadow_depth_depth_tex);
    m_shadow_opacity_FBO.attach_color_texture(m_shadow_opacity_color_tex);
    return true;
}

bool OpenGLFrameBuffers::create_buffers(int width, int height)
{
    m_frame_width = width;
    m_frame_height = height;
    m_shadow_width = 1024;  // fixed, use this in mesh and strands shader program
    m_shadow_height = 1024;

    create_textures();
    create_FBO();
    attach_FBO_textures();

    return true;
}