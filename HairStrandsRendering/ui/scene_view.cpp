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
    int frame_width, frame_height;
    m_frame_buffers->get_comp_FBO().get_texture_size(frame_width, frame_height);
    glViewport(0, 0, frame_width, frame_height);
    
    m_frame_buffers->get_comp_FBO().bind_FBO();
    m_comp_shader->use();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_frame_buffers->get_strands_FBO().get_color_texture().bind_texture_unit(0);
    m_frame_buffers->get_mesh_FBO().get_color_texture().bind_texture_unit(1);
    m_frame_buffers->get_transparency_slab_FBO().get_color_texture().bind_texture_unit(2);

    m_comp_shader->set_i1(0, "strands_map");
    m_comp_shader->set_i1(1, "mesh_map");
    m_comp_shader->set_i1(2, "slab_map");
    
    m_comp_shader->set_f1(m_render_param->strands_alpha, "alpha");
    m_comp_shader->set_f1(m_render_param->gamma, "gamma");
    m_comp_shader->set_f3(m_render_param->clear_color[0],
                          m_render_param->clear_color[1],
                          m_render_param->clear_color[2], "clear_color");

    m_comp->render();

    m_frame_buffers->get_strands_FBO().get_color_texture().unbind_texture_unit(0);
    m_frame_buffers->get_mesh_FBO().get_color_texture().unbind_texture_unit(1);
    m_frame_buffers->get_transparency_slab_FBO().get_color_texture().unbind_texture_unit(2);

    m_comp_shader->disuse();
    m_frame_buffers->get_comp_FBO().unbind_FBO();

    ImGui::Begin("Scene");  // can move to the top of the function
    // add rendered texture to ImGUI scene window
    uint32_t texture_id = m_frame_buffers->get_comp_FBO().get_color_texture().get_texture();
    ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{ m_size.x, m_size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    ImGui::End();
}

void SceneView::render_mesh_depth()
{
    ImGui::Begin("Scene");
    ImVec2 viewport_panelsize = ImGui::GetContentRegionAvail();
    m_size = { viewport_panelsize.x, viewport_panelsize.y };
    m_camera->setup(m_size.x, m_size.y);
    ImGui::End();

    int frame_width, frame_height;
    m_frame_buffers->get_strands_FBO().get_texture_size(frame_width, frame_height);
    glViewport(0, 0, frame_width, frame_height);

    m_frame_buffers->get_strands_FBO().bind_FBO();
    m_mesh_depth_shader->use();

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0, 1.0);

    if (m_mesh)
        m_mesh->render(false);

    m_camera->update_mat(m_mesh_depth_shader.get());

    glDisable(GL_DEPTH_TEST);

    m_mesh_depth_shader->disuse();
    m_frame_buffers->get_strands_FBO().unbind_FBO();

    // // validate mesh color frame
    // ImGui::Begin("Scene");  // can move to the top of the function
    // // add rendered texture to ImGUI scene window
    // uint32_t texture_id = m_frame_buffers->get_strands_FBO().get_depth_texture().get_texture();
    // ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{ m_size.x, m_size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    // ImGui::End();
}

void SceneView::render_transparency()
{
    ImGui::Begin("Scene");  // can move to the top of the function
    ImVec2 viewport_panelsize = ImGui::GetContentRegionAvail();
    m_size = { viewport_panelsize.x, viewport_panelsize.y };
    m_camera->setup(m_size.x, m_size.y);
    ImGui::End();

    // render depth range
    int frame_width, frame_height;
    m_frame_buffers->get_transparency_depth_range_FBO().get_texture_size(frame_width, frame_height);
    glViewport(0, 0, frame_width, frame_height);
    
    m_frame_buffers->get_transparency_depth_range_FBO().bind_FBO();
    m_depth_range_shader->use();

    glEnable(GL_DEPTH_TEST);    // the depth is only read
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);  // blend the color for get the range
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_MIN);
    glClearColor(1e30f, 1e30f, 1e30f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (m_strands)
        m_strands->render(false, m_render_param->strands_width);

    m_camera->update_mat(m_depth_range_shader.get());

    glDepthMask(GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    
    m_depth_range_shader->disuse();
    m_frame_buffers->get_transparency_depth_range_FBO().unbind_FBO();

    // render occupancy map
    m_frame_buffers->get_transparency_occupancy_FBO().get_texture_size(frame_width, frame_height);
    glViewport(0, 0, frame_width, frame_height);
    
    m_frame_buffers->get_transparency_occupancy_FBO().bind_FBO();
    m_occ_shader->use();

    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_OR);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    
    m_frame_buffers->get_transparency_depth_range_FBO().get_color_texture().bind_texture_unit(0);
    m_occ_shader->set_i1(0, "depth_range_map");
    m_occ_shader->set_i1(frame_width, "width");
    m_occ_shader->set_i1(frame_height, "height");

    if (m_strands)
        m_strands->render(false, m_render_param->strands_width);

    m_camera->update_mat(m_occ_shader.get());
    m_frame_buffers->get_transparency_depth_range_FBO().get_color_texture().unbind_texture_unit(0);
    
    glDepthMask(GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_LOGIC_OP);

    m_occ_shader->disuse();
    m_frame_buffers->get_transparency_occupancy_FBO().unbind_FBO();

    // render slab map
    m_frame_buffers->get_transparency_slab_FBO().get_texture_size(frame_width, frame_height);
    glViewport(0, 0, frame_width, frame_height);
    
    m_frame_buffers->get_transparency_slab_FBO().bind_FBO();
    m_slab_shader->use();

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    m_frame_buffers->get_transparency_depth_range_FBO().get_color_texture().bind_texture_unit(0);
    m_slab_shader->set_i1(0, "depth_range_map");
    m_slab_shader->set_i1(frame_width, "width");
    m_slab_shader->set_i1(frame_height, "height");

    if (m_strands)
        m_strands->render(false, m_render_param->strands_width);

    m_camera->update_mat(m_slab_shader.get());
    m_frame_buffers->get_transparency_depth_range_FBO().get_color_texture().unbind_texture_unit(0);
    
    glDepthMask(GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    m_slab_shader->disuse();
    m_frame_buffers->get_transparency_slab_FBO().unbind_FBO();

    // // validate rendering
    // ImGui::Begin("Scene");
    // // add rendered texture to ImGUI scene window
    // uint32_t texture_id = m_frame_buffers->get_transparency_slab_FBO().get_color_texture().get_texture();
    // ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{ m_size.x, m_size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    // ImGui::End();
}

void SceneView::render_shadow()
{   
    int frame_width, frame_height;
    m_frame_buffers->get_shadow_depth_FBO().get_texture_size(frame_width, frame_height);
    glViewport(0, 0, frame_width, frame_height);

    m_lights->update(nullptr);
    
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
        
        m_shadow_depth_shader->set_mat4(m_camera->get_model_mat(), "model_mat");
        m_shadow_depth_shader->set_mat4(m_lights->m_view_mat[i_light], "view_mat");
        m_shadow_depth_shader->set_mat4(m_lights->m_proj_mat[i_light], "proj_mat");

        if (m_strands)
            m_strands->render(false, m_render_param->strands_width);
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

    // render mesh depth with lights
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
        
        m_shadow_depth_shader->set_mat4(m_camera->get_model_mat(), "model_mat");
        m_shadow_depth_shader->set_mat4(m_lights->m_view_mat[i_light], "view_mat");
        m_shadow_depth_shader->set_mat4(m_lights->m_proj_mat[i_light], "proj_mat");

        if (m_mesh)
            m_mesh->render(false);
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
        
        m_shadow_opacity_shader->set_mat4(m_camera->get_model_mat(), "model_mat");
        m_shadow_opacity_shader->set_mat4(m_lights->m_view_mat[i_light], "view_mat");
        m_shadow_opacity_shader->set_mat4(m_lights->m_proj_mat[i_light], "proj_mat");

        if (m_strands)
            m_strands->render(false, m_render_param->strands_width);
    }
    
    m_frame_buffers->get_shadow_depth_FBO().get_color_texture().unbind_texture_unit(0);
    glDisable(GL_DEPTH_TEST);

    m_shadow_opacity_shader->disuse();
    m_frame_buffers->get_shadow_opacity_FBO().unbind_FBO();

    // // validate rendering
    // ImGui::Begin("Scene");
    // // add rendered texture to ImGUI scene window
    // uint32_t texture_id = m_frame_buffers->get_shadow_depth_FBO().get_color_texture().get_texture();
    // ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{ m_size.x, m_size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    // ImGui::End();
}

void SceneView::render_mesh()
{
    ImGui::Begin("Scene");
    ImVec2 viewport_panelsize = ImGui::GetContentRegionAvail();
    m_size = { viewport_panelsize.x, viewport_panelsize.y };
    m_camera->setup(m_size.x, m_size.y);
    ImGui::End();

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
    
    m_mesh_shader->set_f1(m_render_param->mesh_ambient, "Ka");
    m_mesh_shader->set_f1(m_render_param->mesh_diffuse, "Kd");
    m_mesh_shader->set_f1(m_render_param->mesh_specular, "Ks");
    
    m_mesh_shader->set_f1(m_render_param->mesh_self_shadow, "mesh_shadow");
    m_mesh_shader->set_f1(m_render_param->mesh_strands_shadow, "strands_shadow");
    
    m_mesh_shader->set_f3(m_render_param->mesh_color[0], m_render_param->mesh_color[1], m_render_param->mesh_color[2], "color");
    
    if (m_mesh)
        m_mesh->render();

    m_camera->update(m_mesh_shader.get());

    m_frame_buffers->get_shadow_depth_FBO().get_color_texture().unbind_texture_unit(1);
    m_frame_buffers->get_shadow_opacity_FBO().get_color_texture().unbind_texture_unit(2);

    glDisable(GL_DEPTH_TEST);

    m_mesh_shader->disuse();
    m_frame_buffers->get_mesh_FBO().unbind_FBO();

    // // validate mesh color frame
    // ImGui::Begin("Scene");  // can move to the top of the function
    // // add rendered texture to ImGUI scene window
    // uint32_t texture_id = m_frame_buffers->get_mesh_FBO().get_color_texture().get_texture();
    // ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{ m_size.x, m_size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    // ImGui::End();
}

void SceneView::render_strands()
{
    ImGui::Begin("Scene");
    ImVec2 viewport_panelsize = ImGui::GetContentRegionAvail();
    m_size = { viewport_panelsize.x, viewport_panelsize.y };
    m_camera->setup(m_size.x, m_size.y);
    ImGui::End();

    int frame_width, frame_height;
    m_frame_buffers->get_strands_FBO().get_texture_size(frame_width, frame_height);
    glViewport(0, 0, frame_width, frame_height);

    m_frame_buffers->get_strands_FBO().bind_FBO();
    m_strands_shader->use();

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    // glClearDepth(1.f);               // without render mesh depth, clearing depth buffer is required
    // glClear(GL_DEPTH_BUFFER_BIT);    // uncomment it when call render_strands() only
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    m_frame_buffers->get_shadow_depth_FBO().get_color_texture().bind_texture_unit(1);
    m_frame_buffers->get_shadow_opacity_FBO().get_color_texture().bind_texture_unit(2);
    m_frame_buffers->get_transparency_depth_range_FBO().get_color_texture().bind_texture_unit(3);
    m_frame_buffers->get_transparency_occupancy_FBO().get_color_texture().bind_texture_unit(4);
    m_frame_buffers->get_transparency_slab_FBO().get_color_texture().bind_texture_unit(5);

    m_strands_shader->set_f3(m_render_param->strands_color[0],
                             m_render_param->strands_color[1],
                             m_render_param->strands_color[2], "color");

    m_strands_shader->set_i1(1, "depth_map");
    m_strands_shader->set_i1(2, "opacity_map");
    m_strands_shader->set_i1(3, "depth_range_map");
    m_strands_shader->set_i1(4, "occupancy_map");
    m_strands_shader->set_i1(5, "slab_map");

    m_strands_shader->set_vec3(m_lights->m_dirs[0], "light_dir_1");
    m_strands_shader->set_vec3(m_lights->m_dirs[1], "light_dir_2");
    m_strands_shader->set_vec3(m_lights->m_dirs[2], "light_dir_3");
    m_strands_shader->set_vec3(m_lights->m_dirs[3], "light_dir_4");

    m_strands_shader->set_vec3(m_lights->m_colors[0], "light_color_1");
    m_strands_shader->set_vec3(m_lights->m_colors[1], "light_color_2");
    m_strands_shader->set_vec3(m_lights->m_colors[2], "light_color_3");
    m_strands_shader->set_vec3(m_lights->m_colors[3], "light_color_4");

    m_strands_shader->set_mat4(m_lights->m_view_mat[0], "light_view_mat_1");
    m_strands_shader->set_mat4(m_lights->m_view_mat[1], "light_view_mat_2");
    m_strands_shader->set_mat4(m_lights->m_view_mat[2], "light_view_mat_3");
    m_strands_shader->set_mat4(m_lights->m_view_mat[3], "light_view_mat_4");

    m_strands_shader->set_mat4(m_lights->m_view_proj_mat[0], "light_view_proj_mat_1");
    m_strands_shader->set_mat4(m_lights->m_view_proj_mat[1], "light_view_proj_mat_2");
    m_strands_shader->set_mat4(m_lights->m_view_proj_mat[2], "light_view_proj_mat_3");
    m_strands_shader->set_mat4(m_lights->m_view_proj_mat[3], "light_view_proj_mat_4");
    
    m_strands_shader->set_f1(m_render_param->strands_ambient, "Ka");
    m_strands_shader->set_f1(m_render_param->strands_diffuse, "Kd");
    m_strands_shader->set_f1(m_render_param->strands_specular, "Ks");
    
    m_strands_shader->set_f1(m_render_param->strands_alpha, "alpha");
    m_strands_shader->set_f1(m_render_param->strands_self_shadow, "strands_shadow");
    m_strands_shader->set_f1(m_render_param->strands_mesh_shadow, "mesh_shadow");
    
    m_strands_shader->set_i1(frame_width, "width");
    m_strands_shader->set_i1(frame_height, "height");

    if (m_strands)
        m_strands->render(true, m_render_param->strands_width);

    m_camera->update(m_strands_shader.get());

    m_frame_buffers->get_shadow_depth_FBO().get_color_texture().unbind_texture_unit(1);
    m_frame_buffers->get_shadow_opacity_FBO().get_color_texture().unbind_texture_unit(2);
    m_frame_buffers->get_transparency_depth_range_FBO().get_color_texture().unbind_texture_unit(3);
    m_frame_buffers->get_transparency_occupancy_FBO().get_color_texture().unbind_texture_unit(4);
    m_frame_buffers->get_transparency_slab_FBO().get_color_texture().unbind_texture_unit(5);
    
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    m_strands_shader->disuse();
    m_frame_buffers->get_strands_FBO().unbind_FBO();

    // // validate mesh color frame
    // ImGui::Begin("Scene");  // can move to the top of the function
    // // add rendered texture to ImGUI scene window
    // uint32_t texture_id = m_frame_buffers->get_strands_FBO().get_color_texture().get_texture();
    // ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2{ m_size.x, m_size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    // ImGui::End();
}