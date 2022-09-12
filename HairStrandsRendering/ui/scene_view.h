#pragma once

#include "elems/input.h"
#include "elems/camera.h"
#include "elems/light.h"
#include "elems/mesh.h"
#include "elems/strands.h"
#include "render/shader.h"
#include "render/opengl_buffer_manager.h"

class SceneView
{
public:
    SceneView() : 
        m_camera(nullptr), m_light(nullptr), m_size(1600, 1600),
        m_face_frame_buffer(nullptr), m_face_shader(nullptr),
        m_strands_frame_buffer(nullptr), m_strands_shader(nullptr),
        m_comp_frame_buffer(nullptr), m_comp_shader(nullptr)
    {
        m_face_frame_buffer = std::make_unique<OpenGLFrameBuffer>();
        m_face_frame_buffer->create_buffers(m_size.x, m_size.y);
        m_face_shader = std::make_unique<Shader>();
        m_face_shader->load("shaders/vert_mesh.vert", "shaders/frag_pbr_mesh.frag");

        m_strands_frame_buffer = std::make_unique<OpenGLFrameBuffer>();
        m_strands_frame_buffer->create_buffers(m_size.x, m_size.y);
        m_strands_shader = std::make_unique<Shader>();
        m_strands_shader->load("shaders/vert_strands.vert", "shaders/frag_strands.frag");

        m_comp_frame_buffer = std::make_unique<OpenGLFrameBuffer>();
        m_comp_frame_buffer->create_buffers(m_size.x, m_size.y);
        m_comp_shader = std::make_unique<Shader>();
        m_comp_shader->load("shaders/vert_composite.vert", "shaders/frag_composite.frag");

        m_light = std::make_unique<Light>();
        m_camera = std::make_unique<Camera>(glm::vec3(0, 0, 3), 45.0f, 1.3f, 0.1f, 100.0f);
    }
  
    ~SceneView()
    {
        m_face_shader->unload();
        m_strands_shader->unload();
    }
  
    Light* get_light() { return m_light.get(); }
  
    void resize(int32_t width, int32_t height);
  
    void render();
  
    void load_mesh(const std::string& filepath);
    void load_strands(const std::string& filepath);
  
    void set_mesh(std::shared_ptr<Mesh> mesh)
    {
        m_mesh = mesh;
    }
    void set_strands(std::shared_ptr<Strands> strands)
    {
        m_strands = strands;
    }
  
    std::shared_ptr<Mesh> get_mesh() { return m_mesh; }
    
    void on_mouse_move(double x, double y, InputButton button);
  
    void on_mouse_wheel(double delta);
  
    void reset_view()
    {
        m_camera->reset();
    }

private:
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Light> m_light;
    glm::vec2 m_size;

    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Strands> m_strands;

    std::unique_ptr<OpenGLFrameBuffer> m_face_frame_buffer;
    std::unique_ptr<OpenGLFrameBuffer> m_strands_frame_buffer;
    std::unique_ptr<OpenGLFrameBuffer> m_comp_frame_buffer;
    std::unique_ptr<Shader> m_face_shader;
    std::unique_ptr<Shader> m_strands_shader;
    std::unique_ptr<Shader> m_comp_shader;
};