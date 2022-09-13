#include "render/opengl_buffer_manager.h"
#include "glad/glad.h"

void OpenGLVertexIndexBuffer::create_buffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    glGenVertexArrays(1, &m_VAO);
  
    glGenBuffers(1, &m_IBO);
    glGenBuffers(1, &m_VBO);
  
    glBindVertexArray(m_VAO);
  
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
  
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
  
    glBindVertexArray(0);

}

void OpenGLVertexIndexBuffer::delete_buffers()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &m_IBO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void OpenGLVertexIndexBuffer::bind()
{
    glBindVertexArray(m_VAO);
}

void OpenGLVertexIndexBuffer::unbind()
{
    glBindVertexArray(0);
}

void OpenGLVertexIndexBuffer::draw(int index_count)
{
    bind();

    // the vertices as line loop
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);

    unbind();
}

// TODO OpenGLStrandsIndexBuffer
void OpenGLStrandsIndexBuffer::create_buffers(const std::vector<Strand>& strands,
        const std::vector<std::vector<unsigned int>>& indices)
{
}

void OpenGLStrandsIndexBuffer::delete_buffers()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &m_IBO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void OpenGLStrandsIndexBuffer::bind()
{
    glBindVertexArray(m_VAO);
}

void OpenGLStrandsIndexBuffer::unbind()
{
    glBindVertexArray(0);
}

void OpenGLStrandsIndexBuffer::draw(int index_count)
{
    bind();

    // the vertices as line loop
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);

    unbind();
}


void OpenGLFrameBuffer::create_buffers(int32_t width, int32_t height)
{
    m_width = width;
    m_height = height;

    if (m_FBO)
        delete_buffers();

    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_tex_id);  // bug on macos
    glBindTexture(GL_TEXTURE_2D, m_tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_id, 0);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_id);
    glBindTexture(GL_TEXTURE_2D, m_depth_id);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_width, m_height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth_id, 0);

    GLenum buffers[4] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(m_tex_id, buffers);

    unbind();
}

void OpenGLFrameBuffer::delete_buffers()
{
    if (m_FBO)
    {
        glDeleteFramebuffers(GL_FRAMEBUFFER, &m_FBO);
        glDeleteTextures(1, &m_tex_id);
        glDeleteTextures(1, &m_depth_id);
        m_tex_id = 0;
        m_depth_id = 0;
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
    return m_tex_id;
}