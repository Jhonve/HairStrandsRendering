#pragma once

#include "elems/mesh.h"
#include "elems/camera.h"
#include "elems/light.h"
#include "render/shader.h"

#include "render/ui_context.h"
#include "render/opengl_context.h"
#include "render/opengl_buffer_manager.h"

#include "window.h"

#include "ui/property_panel.h"
#include "ui/scene_view.h"

class GLWindow : public ImWindow
{
public:
    GLWindow() :
        m_is_running(true), m_window(nullptr)
    {
        m_UI_ctx = std::make_unique<UIContext>();
        m_render_ctx = std::make_unique<OpenGLContext>();
    }
  
    ~GLWindow();
  
    bool init(int width, int height, const std::string& title);
  
    void render();
  
    void handle_input();
  
    void* get_native_window() override { return m_window; }
  
    void set_native_window(void* window)
    {
        m_window = (GLFWwindow*)window;
    }
  
    void on_scroll(double delta) override;
  
    void on_key(int key, int scancode, int action, int mods) override;
  
    void on_resize(int width, int height) override;
  
    void on_close() override;
  
    bool is_running() { return m_is_running; }


private:
    bool m_is_running;
    GLFWwindow* m_window;

    // Render contexts
    std::unique_ptr<UIContext> m_UI_ctx;

    std::unique_ptr<OpenGLContext> m_render_ctx;

    // UI components
    std::unique_ptr<PropertyPanel> m_property_pannel;

    std::unique_ptr<SceneView> m_scene_view;
};