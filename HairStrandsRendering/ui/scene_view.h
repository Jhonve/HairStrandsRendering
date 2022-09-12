#pragma once

#include "elems/camera.h"
#include "elems/mesh.h"
#include "elems/light.h"
#include "render/shader.h"
#include "render/opengl_buffer_manager.h"
#include "elems/input.h"

class SceneView
{
public:
    SceneView() : 
        m_camera(nullptr), m_frame_buffer(nullptr), m_shader(nullptr), 
        m_light(nullptr), m_size(800, 600)
    {
        m_frame_buffer = std::make_unique<OpenGLFrameBuffer>();
        m_frame_buffer->create_buffers(800, 600);
        m_shader = std::make_unique<Shader>();
        m_shader->load("shaders/vs.shader", "shaders/fs_pbr.shader");
        m_light = std::make_unique<Light>();
  
        m_camera = std::make_unique<Camera>(glm::vec3(0, 0, 3), 45.0f, 1.3f, 0.1f, 100.0f);
  
    }
  
    ~SceneView()
    {
        m_shader->unload();
    }
  
    Light* get_light() { return m_light.get(); }
  
    void resize(int32_t width, int32_t height);
  
    void render();
  
    void load_mesh(const std::string& filepath);
  
    void set_mesh(std::shared_ptr<Mesh> mesh)
    {
        m_mesh = mesh;
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
    std::unique_ptr<OpenGLFrameBuffer> m_frame_buffer;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Light> m_light;
    std::shared_ptr<Mesh> m_mesh;
    glm::vec2 m_size;
};