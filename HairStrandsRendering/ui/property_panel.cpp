#include "property_panel.h"
#include "ui/imguiplugins/ImGuiFileDialog.h"

void PropertyPanel::render(SceneView* scene_view)
{
    // auto mesh = scene_view->get_mesh();
    auto render_param = scene_view->get_render_param();
  
    ImGui::Begin("Properties");
    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Open Mesh"))
        {
            m_mesh_loading = true;
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File",
                                                    ".ply,.fbx,.obj,.off", "../Data/", 1, nullptr, ImGuiFileDialogFlags_Modal);
        }
        ImGui::SameLine(0, 5.0f);
        ImGui::Text(m_mesh_current_file.c_str());
    }

    if (ImGui::CollapsingHeader("Strands", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Open Strands"))
        {
            m_strands_loading = true;
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File",
                                                    ".bin,.data", "../Data/", 1, nullptr, ImGuiFileDialogFlags_Modal);
        }
        ImGui::SameLine(0, 5.0f);
        ImGui::Text(m_strands_current_file.c_str());
    }
  
    if (ImGui::CollapsingHeader("Scene"))
    {
        ImGui::ColorEdit3("Clear Color", (float*)&render_param->clear_color);
    }

    if (ImGui::CollapsingHeader("Mesh Properties") && render_param)
    {
        ImGui::ColorPicker3("Mesh Color", (float*)&render_param->mesh_color, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_DisplayRGB);
        ImGui::SliderFloat("Mesh Diffuse", &render_param->mesh_diffuse, 0.0f, 1.0f);
        ImGui::SliderFloat("Mesh Specular", &render_param->mesh_specular, 0.0f, 1.0f);
        ImGui::SliderFloat("Mesh Ambient", &render_param->mesh_ambient, 0.0f, 1.0f);
    }
  
    if (ImGui::CollapsingHeader("Strands Properties") && render_param)
    {
        ImGui::SliderInt("Strs Width", &render_param->strands_width, 1, 10);
        ImGui::SliderFloat("Strs Diffuse", &render_param->strands_diffuse, 0.0f, 1.0f);
        ImGui::SliderFloat("Strs Specular", &render_param->strands_specular, 0.0f, 1.0f);
        ImGui::SliderFloat("Strs Ambient", &render_param->strands_ambient, 0.0f, 1.0f);
    }

    if (ImGui::CollapsingHeader("Light"))
    {
        ImGui::Separator();
        ImGui::Text("Position");
        ImGui::Separator();
        // draw_vec3_widget("Position", scene_view->get_light()->m_position, 80.0f);
    }
  
    ImGui::End();
  
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) 
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string file_name = ImGuiFileDialog::Instance()->GetFilePathName();
            if(m_mesh_loading)
            {
                m_mesh_current_file = file_name.substr(file_name.find_last_of("/\\") + 1);
                m_mesh_loading = false;
                m_mesh_load_callback(file_name);
            }
            else if (m_strands_loading)
            {
                m_strands_current_file = file_name.substr(file_name.find_last_of("/\\") + 1);
                m_strands_loading = false;
                m_strands_load_callback(file_name);
            }
            
        }
        ImGuiFileDialog::Instance()->Close();
    }
}