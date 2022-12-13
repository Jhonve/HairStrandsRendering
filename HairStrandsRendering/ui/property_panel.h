#pragma once

#include "elems/light.h"
#include "ui/scene_view.h"

#include "imgui.h"
#include "ui/utils_widgets.h"

#include <functional>


class PropertyPanel
{
public:
    PropertyPanel()
    {
        m_mesh_current_file = "< ... >";
        m_strands_current_file = "< ... >";
    }

    void render(SceneView* mScene);

    void set_mesh_load_callback(const std::function<void(const std::string&)>& callback)
    {
        m_mesh_load_callback = callback;
    }

    void set_strands_load_callback(const std::function<void(const std::string&)>& callback)
    {
        m_strands_load_callback = callback;
    }


private:
    // create a file browser instance
    std::string m_mesh_current_file;
    std::function<void(const std::string&)> m_mesh_load_callback;

    std::string m_strands_current_file;
    std::function<void(const std::string&)> m_strands_load_callback;

    bool m_mesh_loading = false;
    bool m_strands_loading = false;

    // strands processing parameters
    float m_smooth_lap_const = 4.0f;
    float m_smooth_pos_const = 2.0f;
    float m_downsample_sim_thres = 0.98f;
    int m_num_interp_pts = 10;
    int m_dup_ratio = 10;
    float m_dup_perturbation = 1.0;
};