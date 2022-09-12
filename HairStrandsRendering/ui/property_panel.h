#pragma once

#include "elems/light.h"
#include "ui/scene_view.h"

#include "imgui.h"
#include "ui/utils_imgui_widgets.h"
#include "ui/imguiplugins/ImFileBrowser.h"

class PropertyPanel
{
public:

    PropertyPanel()
    {
        m_current_file = "< ... >";

        m_file_dialog.SetTitle("Open mesh");
        m_file_dialog.SetFileFilters({ ".fbx", ".obj", ".ply", ".off"});
    }

    void render(SceneView* mScene);

    void set_mesh_load_callback(const std::function<void(const std::string&)>& callback)
    {
        m_mesh_load_callback = callback;
    }

private:
    // create a file browser instance
    ImGui::FileBrowser m_file_dialog;

    std::function<void(const std::string&)> m_mesh_load_callback;

    std::string m_current_file;
};