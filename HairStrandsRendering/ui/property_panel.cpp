#include "property_panel.h"
#include "ui/imguiplugins/ImGuiFileDialog.h"

void PropertyPanel::render(SceneView* scene_view)
{
    auto render_param = scene_view->get_render_param();
  
    ImGui::Begin("Properties");

    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Open Mesh"))
        {
            m_mesh_loading = true;
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File",
                                                    ".ply,.fbx,.obj,.off", "../Data/", 1, nullptr, ImGuiFileDialogFlags_Modal);
        }
        ImGui::SameLine(0, 5.0f);
        if (ImGui::Button("Clear Mesh"))
        {
            if (scene_view->get_mesh())
                scene_view->del_mesh();
            m_mesh_current_file = "< ... >";
        }

        ImGui::Text(m_mesh_current_file.c_str());
    }

    if (ImGui::CollapsingHeader("Strands", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Open Strands"))
        {
            m_strands_loading = true;
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File",
                                                    ".bin,.cin,.data,.hair", "../Data/", 1, nullptr, ImGuiFileDialogFlags_Modal);
        }
        ImGui::SameLine(0, 5.0f);
        if (ImGui::Button("Clear Strands"))
        {
            if(scene_view->get_strands())
                scene_view->del_strands();
            m_strands_current_file = "< ... >";
        }

        ImGui::Text(m_strands_current_file.c_str());

        if (ImGui::Button("Save as .bin"))
        {
            if(scene_view->get_strands())
                scene_view->get_strands()->save(".bin");
        }
        ImGui::SameLine(0, 5.0f);
        if (ImGui::Button("Save as .data"))
        {
            if(scene_view->get_strands())
                scene_view->get_strands()->save(".data");
        }
        
        ImGui::Text("Strands Processing");
        if(scene_view->get_strands())
        {
            std::string show_nums = "Num strs: " + std::to_string(scene_view->get_strands()->get_num_strands()) + 
                        " Num pts: " + std::to_string(scene_view->get_strands()->get_num_points());
            ImGui::Text(show_nums.c_str());
        }

        ImGui::PushItemWidth(64);
        ImGui::InputFloat("Lap", &m_smooth_lap_const);
        ImGui::SameLine(0, 5.0f);
        ImGui::InputFloat("Pos", &m_smooth_pos_const);
        ImGui::PopItemWidth();
        ImGui::SameLine(0, 5.0f);
        if (ImGui::Button("Smooth"))
        {
            if(scene_view->get_strands())
                scene_view->get_strands()->smooth(m_smooth_lap_const, m_smooth_pos_const);
        }

        ImGui::PushItemWidth(80);
        ImGui::InputFloat("Similarity", &m_downsample_sim_thres);
        ImGui::PopItemWidth();
        ImGui::SameLine(0, 5.0f);
        if (ImGui::Button("Downsample"))
        {
            if(scene_view->get_strands())
                scene_view->get_strands()->downsample(m_downsample_sim_thres);
        }

        ImGui::PushItemWidth(80);
        ImGui::InputInt("Number Pts", &m_num_interp_pts);
        ImGui::PopItemWidth();
        ImGui::SameLine(0, 5.0f);
        if (ImGui::Button("Parametric"))
        {
            if(scene_view->get_strands())
                scene_view->get_strands()->parametrical(m_num_interp_pts);
        }
        if (ImGui::Button("Save Smoothed"))
        {
            if(scene_view->get_strands())
                scene_view->get_strands()->save("smoothed");
        }
        ImGui::SameLine(0, 5.0f);
        if (ImGui::Button("Save Downsampled"))
        {
            if(scene_view->get_strands())
                scene_view->get_strands()->save("downsampled");
        }
        
        ImGui::Text("Strands Duplication");

        ImGui::PushItemWidth(70);
        ImGui::InputInt("Dup", &m_dup_ratio);
        ImGui::PopItemWidth();
        ImGui::SameLine(0, 5.0f);
        ImGui::PushItemWidth(50);
        ImGui::InputFloat("Pert", &m_dup_perturbation);
        ImGui::PopItemWidth();
        ImGui::SameLine(0, 5.0f);
        if (ImGui::Button("Duplicate"))
        {
            if(scene_view->get_strands())
                scene_view->get_strands()->duplicate(m_dup_ratio, m_dup_perturbation);
        }
        if (ImGui::Button("Save Duplicated"))
        {
            if(scene_view->get_strands())
                scene_view->get_strands()->save("duplicated");
        }
    }
  
    if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen))
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
  
    if (ImGui::CollapsingHeader("Strands Properties", ImGuiTreeNodeFlags_DefaultOpen) && render_param)
    {
        ImGui::ColorPicker3("Strs Color", (float*)&render_param->strands_color, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_DisplayRGB);
        ImGui::SliderInt("Strs Width", &render_param->strands_width, 1, 10);
        ImGui::SliderFloat("Strs Alpha", &render_param->strands_alpha, 0.0f, 1.0f);
        ImGui::SliderFloat("Strs Diffuse", &render_param->strands_diffuse, 0.0f, 1.0f);
        ImGui::SliderFloat("Strs Specular", &render_param->strands_specular, 0.0f, 1.0f);
        ImGui::SliderFloat("Strs Ambient", &render_param->strands_ambient, 0.0f, 1.0f);
    }

    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat("Strength", &scene_view->get_lights()->m_strength, 0.f, 1.f);
        ImGui::SliderFloat3("Rot", scene_view->get_lights()->m_rot, 0.f, 3.1415f);
    }
    
    if (ImGui::CollapsingHeader("Shadow Properties", ImGuiTreeNodeFlags_DefaultOpen) && render_param)
    {
        ImGui::SliderFloat("Mesh Self", &render_param->mesh_self_shadow, 0.0f, 0.95f);
        ImGui::SliderFloat("Mesh Strs", &render_param->mesh_strands_shadow, 0.0f, 0.95f);
        ImGui::SliderFloat("Strs Self", &render_param->strands_self_shadow, 0.0f, 0.95f);
        ImGui::SliderFloat("Strs Mesh", &render_param->strands_mesh_shadow, 0.0f, 0.95f);
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
                m_mesh_load_callback(file_name);
            }
            else if (m_strands_loading)
            {
                m_strands_current_file = file_name.substr(file_name.find_last_of("/\\") + 1);
                m_strands_load_callback(file_name);
            }
            
        }
        m_mesh_loading = false;
        m_strands_loading = false;
        ImGuiFileDialog::Instance()->Close();
    }
}