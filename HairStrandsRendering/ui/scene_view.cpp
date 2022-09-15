#include <glad/glad.h>
#include "ui/scene_view.h"
#include "imgui.h"

void SceneView::resize(int width, int height)
{
    m_size.x = width;
    m_size.y = height;
  
    m_frame_buffers->create_buffers((int)m_size.x, (int) m_size.y);
}

void SceneView::on_mouse_move(double x, double y, InputButton button)
{
    m_camera->on_mouse_move(x, y, button);
}

void SceneView::on_mouse_wheel(double delta)
{
    m_camera->on_mouse_wheel(delta);
}

void SceneView::load_mesh(const std::string& filepath)
{
    if(!m_mesh)
        m_mesh = std::make_shared<Mesh>();
  
    m_mesh->load(filepath);
}

void SceneView::load_strands(const std::string& filepath)
{
    if(!m_strands)
        m_strands = std::make_shared<Strands>();
  
    m_strands->load(filepath);
}

void SceneView::render()
{

}

void SceneView::render_transparency()
{

}

void SceneView::render_shadow()
{   
    int frame_width, frame_height;
    m_frame_buffers->get_shadow_depth_FBO().get_texture_size(frame_width, frame_height);
    glViewport(0, 0, frame_width, frame_height);
    
    // renfer strands depth with lights
    m_frame_buffers->get_shadow_depth_FBO().bind_FBO();
    m_shadow_depth_shader->use();
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthRange(0.0, 1.0);
    glDepthFunc(GL_LEQUAL);
    glClearColor(1e30f, 1e30f, 1e30f, 1e30f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);

    for (int i_light = 0; i_light < m_lights->m_num_lights; i_light++)
    {
        glViewport((i_light % 2) * frame_width / 2,
                   (i_light / 2) * frame_height / 2,
                   frame_width / 2,
                   frame_height / 2);
        
        glm::mat4 model{ 1.0f };
        m_shadow_depth_shader->set_mat4(model, "model_mat");
        m_shadow_depth_shader->set_mat4(m_lights->m_view_mat[i_light], "view_mat");
        m_shadow_depth_shader->set_mat4(m_lights->m_proj_mat[i_light], "proj_mat");

        if (m_strands)
        {
            // TODO
        }
    }

    glDisable(GL_DEPTH_TEST);
    m_shadow_depth_shader->disuse();
    m_frame_buffers->get_shadow_depth_FBO().unbind_FBO();

    // Copy can be replace by glBlitNamedFramebuffer
    m_frame_buffers->get_shadow_depth_FBO().bind_FBO_read();
    m_frame_buffers->get_shadow_opacity_FBO().bind_FBO_draw();
    glBlitFramebuffer(0, 0, frame_width, frame_height, 0, 0, frame_width, frame_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    m_frame_buffers->get_shadow_depth_FBO().unbind_FBO_read();
    m_frame_buffers->get_shadow_opacity_FBO().unbind_FBO_draw();

    // renfer mesh depth with lights
    m_frame_buffers->get_shadow_depth_FBO().bind_FBO();
    m_shadow_depth_shader->use();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthRange(0., 1.);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE);

    for (int i_light = 0; i_light < m_lights->m_num_lights; i_light++)
    {
        glViewport((i_light % 2) * frame_width / 2,
                   (i_light / 2) * frame_height / 2,
                   frame_width / 2,
                   frame_height / 2);
        
        glm::mat4 model{ 1.0f };
        m_shadow_depth_shader->set_mat4(model, "model_mat");
        m_shadow_depth_shader->set_mat4(m_lights->m_view_mat[i_light], "view_mat");
        m_shadow_depth_shader->set_mat4(m_lights->m_proj_mat[i_light], "proj_mat");

        if (m_mesh)
            m_mesh->render();
    }

    glDisable(GL_DEPTH_TEST);
    m_shadow_depth_shader->disuse();
    m_frame_buffers->get_shadow_depth_FBO().unbind_FBO();

    // render shadow map
    m_frame_buffers->get_shadow_opacity_FBO().get_texture_size(frame_width, frame_height);

    m_frame_buffers->get_shadow_opacity_FBO().bind_FBO();
    m_shadow_opacity_shader->use();

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_frame_buffers->get_shadow_depth_FBO().get_color_texture().bind_texture_unit(0);
    m_shadow_opacity_shader->set_i1(0, "depth_map");
    m_shadow_opacity_shader->set_i1(frame_width, "width");
    m_shadow_opacity_shader->set_i1(frame_height, "height");

    for (int i_light = 0; i_light < m_lights->m_num_lights; i_light++)
    {
        glViewport((i_light % 2) * frame_width / 2,
                   (i_light / 2) * frame_height / 2,
                   frame_width / 2,
                   frame_height / 2);
        
        glm::mat4 model{ 1.0f };
        m_shadow_opacity_shader->set_mat4(model, "model_mat");
        m_shadow_opacity_shader->set_mat4(m_lights->m_view_mat[i_light], "view_mat");
        m_shadow_opacity_shader->set_mat4(m_lights->m_proj_mat[i_light], "proj_mat");

        if (m_strands)
        {
            // TODO
        }
    }

    glDisable(GL_DEPTH_TEST);

    m_shadow_opacity_shader->disuse();
    m_frame_buffers->get_shadow_opacity_FBO().unbind_FBO();

    // validate rendering
    // ImGui::Begin("Scene");
    // ImVec2 viewport_panelsize = ImGui::GetContentRegionAvail();
    // m_size = { viewport_panelsize.x, viewport_panelsize.y };

    // // add rendered texture to ImGUI scene window
    // uint32_t texture_id = m_frame_buffers->get_shadow_opacity_FBO().get_color_texture().get_texture();
    // ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{ m_size.x, m_size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    // ImGui::End();
}

void SceneView::render_mesh()
{
    int frame_width, frame_height;
    m_frame_buffers->get_mesh_FBO().get_texture_size(frame_width, frame_height);
    glViewport(0, 0, frame_width, frame_height);

    m_frame_buffers->get_mesh_FBO().bind_FBO();
    m_mesh_shader->use();

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClearDepth(1.0);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    
    m_frame_buffers->get_shadow_depth_FBO().get_color_texture().bind_texture_unit(1);
    m_frame_buffers->get_shadow_opacity_FBO().get_color_texture().bind_texture_unit(2);

    m_mesh_shader->set_i1(1, "depth_map");
    m_mesh_shader->set_i1(2, "opacity_map");

    m_mesh_shader->set_vec3(m_lights->m_dirs[0], "light_dir_1");
    m_mesh_shader->set_vec3(m_lights->m_dirs[1], "light_dir_2");
    m_mesh_shader->set_vec3(m_lights->m_dirs[2], "light_dir_3");
    m_mesh_shader->set_vec3(m_lights->m_dirs[3], "light_dir_4");

    m_mesh_shader->set_vec3(m_lights->m_colors[0], "light_color_1");
    m_mesh_shader->set_vec3(m_lights->m_colors[1], "light_color_2");
    m_mesh_shader->set_vec3(m_lights->m_colors[2], "light_color_3");
    m_mesh_shader->set_vec3(m_lights->m_colors[3], "light_color_4");

    m_mesh_shader->set_mat4(m_lights->m_view_mat[0], "light_view_mat_1");
    m_mesh_shader->set_mat4(m_lights->m_view_mat[1], "light_view_mat_2");
    m_mesh_shader->set_mat4(m_lights->m_view_mat[2], "light_view_mat_3");
    m_mesh_shader->set_mat4(m_lights->m_view_mat[3], "light_view_mat_4");

    m_mesh_shader->set_mat4(m_lights->m_view_proj_mat[0], "light_view_proj_mat_1");
    m_mesh_shader->set_mat4(m_lights->m_view_proj_mat[1], "light_view_proj_mat_2");
    m_mesh_shader->set_mat4(m_lights->m_view_proj_mat[2], "light_view_proj_mat_3");
    m_mesh_shader->set_mat4(m_lights->m_view_proj_mat[3], "light_view_proj_mat_4");
    
    m_mesh_shader->set_f1(m_render_param.mesh_ambient, "Ka");
    m_mesh_shader->set_f1(m_render_param.mesh_diffuse, "Kd");
    m_mesh_shader->set_f1(m_render_param.mesh_specular, "Ks");
    
    m_mesh_shader->set_f1(m_render_param.mesh_self_shadow, "mesh_shadow");
    m_mesh_shader->set_f1(m_render_param.mesh_strands_shadow, "strands_shadow");
    
    m_mesh_shader->set_f3(m_render_param.mesh_color[0], m_render_param.mesh_color[1], m_render_param.mesh_color[2], "color");
    
    if (m_mesh)
        m_mesh->render();

    m_frame_buffers->get_shadow_depth_FBO().get_color_texture().unbind_texture_unit(1);
    m_frame_buffers->get_shadow_opacity_FBO().get_color_texture().unbind_texture_unit(2);

    glDisable(GL_DEPTH_TEST);
    m_frame_buffers->get_mesh_FBO().unbind_FBO();

    ImGui::Begin("Scene");  // can move to the top of the function
    ImVec2 viewport_panelsize = ImGui::GetContentRegionAvail();
    m_size = { viewport_panelsize.x, viewport_panelsize.y };
    ImGui::End();

    m_camera->set_aspect(m_size.x / m_size.y);
    m_camera->update(m_mesh_shader.get());

    m_mesh_shader->disuse();
    
    // // validate mesh color frame
    // ImGui::Begin("Scene");  // can move to the top of the function
    // // add rendered texture to ImGUI scene window
    // uint32_t texture_id = m_frame_buffers->get_mesh_FBO().get_color_texture().get_texture();
    // ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{ m_size.x, m_size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    // ImGui::End();
}

void SceneView::render_strands()
{

}