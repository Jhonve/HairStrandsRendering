#include "glad/glad.h"
#include "ui/scene_view.h"
#include "imgui.h"

void SceneView::resize(int32_t width, int32_t height)
{
    m_size.x = width;
    m_size.y = height;
  
    m_face_frame_buffer->create_buffers((int32_t)m_size.x, (int32_t) m_size.y);
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
    m_face_shader->use();

    m_light->update(m_face_shader.get());

    m_face_frame_buffer->bind();

    if (m_mesh)
    {
        m_mesh->update(m_face_shader.get());
        m_mesh->render();
    }

    m_face_frame_buffer->unbind();

    // TODO use another shader program here
    /*
    * 1. we need a composite shader program including vertex and fragment
    * 2. composite fragment textures to the shader program
    * 3. get_texture from it's frame buffer and draw it
    */

    ImGui::Begin("Scene");

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_size = { viewportPanelSize.x, viewportPanelSize.y };

    m_camera->set_aspect(m_size.x / m_size.y);
    m_camera->update(m_face_shader.get());

    // add rendered texture to ImGUI scene window
    uint64_t textureID = m_face_frame_buffer->get_texture();
    ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_size.x, m_size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

    ImGui::End();
}