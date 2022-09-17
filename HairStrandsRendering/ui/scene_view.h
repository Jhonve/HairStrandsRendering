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
        m_camera(nullptr), m_light(nullptr), m_lights(nullptr), m_size(1600, 1600), m_frame_buffers(nullptr),
        m_mesh_shader(nullptr), m_strands_shader(nullptr), m_comp_shader(nullptr)
    {
        m_frame_buffers = std::make_unique<OpenGLFrameBuffers>();
        m_frame_buffers->create_buffers(m_size.x, m_size.y);
        
        m_depth_range_shader = std::make_unique<Shader>();
        m_depth_range_shader->load("shaders/depth_range.vert", "shaders/depth_range.frag");
        m_occ_shader = std::make_unique<Shader>();
        m_occ_shader->load("shaders/occ.vert", "shaders/occ.frag");
        m_slab_shader = std::make_unique<Shader>();
        m_slab_shader->load("shaders/slab.vert", "shaders/slab.frag");
        m_mesh_depth_shader = std::make_unique<Shader>();
        m_mesh_depth_shader->load("shaders/depth.vert", "shaders/depth.frag");
        m_shadow_depth_shader = std::make_unique<Shader>();
        m_shadow_depth_shader->load("shaders/depth.vert", "shaders/depth.frag");
        m_shadow_opacity_shader = std::make_unique<Shader>();
        m_shadow_opacity_shader->load("shaders/shadow.vert", "shaders/shadow.frag");

        m_mesh_shader = std::make_unique<Shader>();
        m_mesh_shader->load("shaders/mesh_shade.vert", "shaders/mesh_shade.frag");
        m_strands_shader = std::make_unique<Shader>();
        m_strands_shader->load("shaders/strands.vert", "shaders/strands.frag");

        m_comp_shader = std::make_unique<Shader>();
        m_comp_shader->load("shaders/composite.vert", "shaders/composite.frag");
    
        m_light = std::make_unique<Light>();
        m_lights = std::make_unique<Lights>(m_render_param.light);
        m_camera = std::make_unique<Camera>(glm::vec3(0, 0, 3), 45.0f, 1.3f, 0.1f, 100.0f);

        m_comp = std::make_shared<QuadMesh>();
        m_comp->init();
    }
  
    ~SceneView()
    {
        m_depth_range_shader->unload();
        m_occ_shader->unload();
        m_slab_shader->unload();
        m_mesh_depth_shader->unload();
        m_shadow_depth_shader->unload();
        m_shadow_opacity_shader->unload();
        m_mesh_shader->unload();
        m_strands_shader->unload();
        m_comp_shader->unload();
    }
  
    Light* get_light() { return m_light.get(); }
    std::shared_ptr<Mesh> get_mesh() { return m_mesh; }
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
    
    void on_mouse_move(double x, double y, InputButton button);
  
    void on_mouse_wheel(double delta);
  
    void reset_view()
    {
        m_camera->reset();
    }

    void resize(int width, int height);
    void render();
    void render_mesh_depth();
    void render_transparency();
    void render_shadow();
    void render_mesh();
    void render_strands();

private:
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Light> m_light;
    std::unique_ptr<Lights> m_lights;
    glm::vec2 m_size;

    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Strands> m_strands;
    std::shared_ptr<QuadMesh> m_comp;

    std::unique_ptr<OpenGLFrameBuffers> m_frame_buffers;

    std::unique_ptr<Shader> m_depth_range_shader;
    std::unique_ptr<Shader> m_occ_shader;
    std::unique_ptr<Shader> m_slab_shader;
    std::unique_ptr<Shader> m_mesh_depth_shader;
    std::unique_ptr<Shader> m_shadow_depth_shader;
    std::unique_ptr<Shader> m_shadow_opacity_shader;
    std::unique_ptr<Shader> m_mesh_shader;
    std::unique_ptr<Shader> m_strands_shader;
    std::unique_ptr<Shader> m_comp_shader;

    RenderParameters m_render_param;
};