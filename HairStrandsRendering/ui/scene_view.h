#pragma once

#include "elems/input.h"
#include "elems/camera.h"
#include "elems/light.h"
#include "elems/mesh.h"
#include "elems/strands.h"
#include "render/shader.h"
#include "render/buffer_manager.h"

class SceneView
{
public:
    SceneView(int win_width, int win_height)
    {
        m_win_size = glm::vec2(win_width, win_height);
        m_dock_size = glm::vec2(win_width, win_height);

        m_frame_buffers = std::make_unique<OpenGLFrameBuffers>();
        m_frame_buffers->create_buffers(m_win_size.x, m_win_size.y);
        
        m_depth_range_shader = std::make_unique<Shader>();
        m_occ_shader = std::make_unique<Shader>();
        m_slab_shader = std::make_unique<Shader>();
        m_mesh_depth_shader = std::make_unique<Shader>();
        m_shadow_depth_shader = std::make_unique<Shader>();
        m_shadow_opacity_shader = std::make_unique<Shader>();
        m_mesh_shader = std::make_unique<Shader>();
        m_strands_shader = std::make_unique<Shader>();
        m_comp_shader = std::make_unique<Shader>();

#if defined(__APPLE__)
        m_depth_range_shader->load("shaders/macos/depth.macos.vert", "shaders/macos/depth_range.macos.frag");
        m_occ_shader->load("shaders/macos/depth.macos.vert", "shaders/macos/occ.macos.frag");
        m_slab_shader->load("shaders/macos/depth.macos.vert", "shaders/macos/slab.macos.frag");
        m_mesh_depth_shader->load("shaders/macos/depth.macos.vert", "shaders/macos/depth.macos.frag");
        m_shadow_depth_shader->load("shaders/macos/depth.macos.vert", "shaders/macos/depth.macos.frag");
        m_shadow_opacity_shader->load("shaders/macos/depth.macos.vert", "shaders/macos/shadow.macos.frag");
        m_mesh_shader->load("shaders/macos/mesh_shade.macos.vert", "shaders/macos/mesh_shade.macos.frag");
        m_strands_shader->load("shaders/macos/strands.macos.vert", "shaders/macos/strands.macos.frag");
        m_comp_shader->load("shaders/macos/composite.macos.vert", "shaders/macos/composite.macos.frag");
#else
        m_depth_range_shader->load("shaders/depth.vert", "shaders/depth_range.frag");
        m_occ_shader->load("shaders/depth.vert", "shaders/occ.frag");
        m_slab_shader->load("shaders/depth.vert", "shaders/slab.frag");
        m_mesh_depth_shader->load("shaders/depth.vert", "shaders/depth.frag");
        m_shadow_depth_shader->load("shaders/depth.vert", "shaders/depth.frag");
        m_shadow_opacity_shader->load("shaders/depth.vert", "shaders/shadow.frag");
        m_mesh_shader->load("shaders/mesh_shade.vert", "shaders/mesh_shade.frag");
        m_strands_shader->load("shaders/strands.vert", "shaders/strands.frag");
        m_comp_shader->load("shaders/composite.vert", "shaders/composite.frag");
#endif

        m_render_param = std::make_shared<RenderParameters>();
        m_lights = std::make_shared<Lights>(m_render_param->light);
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
  
    std::shared_ptr<Mesh> get_mesh() { return m_mesh; }
    std::shared_ptr<Strands> get_strands() { return m_strands; }
    std::shared_ptr<RenderParameters> get_render_param() { return m_render_param; }
    std::shared_ptr<Lights> get_lights() { return m_lights; }

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
    void del_mesh()
    {
        if(m_mesh)
            m_mesh.reset();
    }
    void del_strands()
    {
        if(m_strands)
            m_strands.reset();
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
    std::shared_ptr<Lights> m_lights;
    std::shared_ptr<RenderParameters> m_render_param;
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

    glm::vec2 m_win_size;
    glm::vec2 m_dock_size;
};