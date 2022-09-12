#include "property_panel.h"

void PropertyPanel::render(SceneView* scene_view)
{
    auto mesh = scene_view->get_mesh();
  
    ImGui::Begin("Properties");
    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Open Mesh"))
            m_mesh_file_dialog.Open();
        ImGui::SameLine(0, 5.0f);
        ImGui::Text(m_mesh_current_file.c_str());
    }

    if (ImGui::CollapsingHeader("Strands", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Open Strands"))
            m_strands_file_dialog.Open();
        ImGui::SameLine(0, 5.0f);
        ImGui::Text(m_strands_current_file.c_str());
    }
  
    if (ImGui::CollapsingHeader("Mesh Material") && mesh)
    {
        ImGui::ColorPicker3("Color", (float*)&mesh->m_color, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_DisplayRGB);
        ImGui::SliderFloat("Roughness", &mesh->m_roughness, 0.0f, 1.0f);
        ImGui::SliderFloat("Metallic", &mesh->m_metallic, 0.0f, 1.0f);
    }
  
    if (ImGui::CollapsingHeader("Light"))
    {
        ImGui::Separator();
        ImGui::Text("Position");
        ImGui::Separator();
        draw_vec3_widget("Position", scene_view->get_light()->m_position, 80.0f);
    }
  
    ImGui::End();
  
    m_mesh_file_dialog.Display();
    if (m_mesh_file_dialog.HasSelected())
    {
        auto file_path = m_mesh_file_dialog.GetSelected().string();
        m_mesh_current_file = file_path.substr(file_path.find_last_of("/\\") + 1);
  
        m_mesh_load_callback(file_path);
  
        m_mesh_file_dialog.ClearSelected();
    }

    m_strands_file_dialog.Display();
    if (m_strands_file_dialog.HasSelected())
    {
        auto file_path = m_strands_file_dialog.GetSelected().string();
        m_strands_current_file = file_path.substr(file_path.find_last_of("/\\") + 1);
  
        m_strands_load_callback(file_path);
  
        m_strands_file_dialog.ClearSelected();
    }
}