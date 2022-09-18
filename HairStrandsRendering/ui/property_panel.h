#pragma once

#include "elems/light.h"
#include "ui/scene_view.h"

#include "imgui.h"
#include "ui/utils_imgui_widgets.h"

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
};