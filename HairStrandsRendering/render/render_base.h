#pragma once

#include "window.h"
#include "elems/vertex.h"
#include <glad/glad.h>

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
  
    virtual void create_buffers(const std::vector<StrandVertex>& vertices,
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

class FrameBuffer
{
public:
    FrameBuffer() : m_FBO { 0 } {}
  
    virtual void create_buffers(int width, int height) = 0;
  
    virtual void delete_buffers() = 0;
  
    virtual void bind() = 0;
  
    virtual void unbind() = 0;
  
    virtual uint32_t get_texture() = 0;

protected:
    uint32_t m_FBO = 0;
    uint32_t m_color_tex_id = 0;
    uint32_t m_depth_tex_id = 0;
    int m_width = 0;
    int m_height = 0;
};

class GLTexture
{
public:
    GLTexture() {}

    bool create_texture()
    {
        glGenTextures(1, &m_tex_id);
        return true;
    }

    bool delete_texture()
    {
        glDeleteTextures(1, &m_tex_id);
        m_tex_id = 0;
        return true;
    }

    GLuint get_texture()
    {
        return m_tex_id;
    }

    void get_texture_size(int& width, int& height) const
    {
        width = m_tex_width;
        height = m_tex_height;
    }

    bool active_texture(int unit_id) const
    {
        glActiveTexture(GL_TEXTURE0 + unit_id);
        return true;
    }

    bool bind_texture() const
    {
        glBindTexture(GL_TEXTURE_2D, m_tex_id);
        return true;
    }

    bool unbind_texture() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }

    bool bind_texture_unit(int unit_id) const
    {
        active_texture(unit_id);
        bind_texture();
        return true;
    }

    bool unbind_texture_unit(int unit_id) const
    {
        active_texture(unit_id);
        unbind_texture();
        return true;
    }

    bool image_texture(GLint level, GLint internal_format, GLsizei width, GLsizei height, GLint border, GLenum image_format, GLenum datatype, const GLvoid* data)
    {
        m_tex_width = width;
        m_tex_height = height;
        glTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height, border, image_format, datatype, data);
        return true;
    }

    bool storage_texture(GLint level, GLint internal_format, GLsizei width, GLsizei height)
    {
        m_tex_width = width;
        m_tex_height = height;
        glTexStorage2D(GL_TEXTURE_2D, level, internal_format, width, height);
        return true;
    }

    bool create_depth_texture(int width, int height, int filter)
    {
        m_tex_width = width;
        m_tex_height = height;
        create_texture();
        bind_texture();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        storage_texture(1, GL_DEPTH24_STENCIL8, width, height);
        unbind_texture();
        return true;
    }

    bool create_R_textture(int width, int height, int filter)
    {
        m_tex_width = width;
        m_tex_height = height;
        create_texture();
        bind_texture();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        image_texture(0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
        unbind_texture();
        return true;
    }

    bool create_R32F_texture(int width, int height, int filter)
    {
        m_tex_width = width;
        m_tex_height = height;
        create_texture();
        bind_texture();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        image_texture(0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);
        unbind_texture();
        return true;
    }

    bool create_RGBA_texture(int width, int height, int filter)
    {
        m_tex_width = width;
        m_tex_height = height;
        create_texture();
        bind_texture();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        image_texture(0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        unbind_texture();
        return true;
    }

    bool create_RGBA32F_texture(int width, int height, int filter)
    {
        m_tex_width = width;
        m_tex_height = height;
        create_texture();
        bind_texture();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        image_texture(0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        unbind_texture();
        return true;
    }

    bool create_RGBA32UI_texture(int width, int height, int filter)
    {
        m_tex_width = width;
        m_tex_height = height;
        create_texture();
        bind_texture();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        image_texture(0, GL_RGBA32UI, width, height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, NULL);
        unbind_texture();
        return true;
    }

private:
    GLuint m_tex_id = 0;
    int m_tex_width = 0;
    int m_tex_height = 0;
};

class GLFBO
{
public:

    GLFBO()
    {
        m_FBOID = 0;
        m_color_tex = nullptr;
        m_depth_tex = nullptr;
    }

    bool create_FBO()
    {
        glGenFramebuffers(1, &m_FBOID);
        // printf("Create FBO Success, ID is %d\n", m_FBOID);
        return m_FBOID != 0;
    }

    bool delete_FBO()
    {
        glDeleteFramebuffers(1, &m_FBOID);
        m_FBOID = 0;
        return true;
    }

    GLuint get_FBO()
    {
        return m_FBOID;
    }

    GLTexture& get_color_texture()
    {
        return *m_color_tex;
    }

    GLTexture& get_depth_texture()
    {
        return *m_depth_tex;
    }

    bool get_texture_size(int& width, int& height)
    {
        if (m_color_tex)
            m_color_tex->get_texture_size(width, height);
        else if (m_depth_tex)
            m_depth_tex->get_texture_size(width, height);
        else
            return false;
        return true;
    }

    bool bind_FBO() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);
        return true;
    }

    bool bind_FBO_read() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBOID);
        return true;
    }

    bool bind_FBO_draw() const
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBOID);
        return true;
    }

    bool unbind_FBO() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }

    bool unbind_FBO_read() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        return true;
    }

    bool unbind_FBO_draw() const
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        return true;
    }

    bool attach_color_texture(GLTexture& color_texture)
    {
        m_color_tex = &color_texture;
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture.get_texture(), 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete for color texture!\n");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }

    bool attach_depth_texture(GLTexture& depth_texture)
    {
        m_depth_tex = &depth_texture;
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_texture.get_texture(), 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete for depth texture!\n");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }

    bool set_depth_only()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBOID);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }

private:
    GLuint m_FBOID;
    GLTexture* m_color_tex;
    GLTexture* m_depth_tex;
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