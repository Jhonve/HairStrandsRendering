#include "viewer.h"
#include "elems/input.h"

bool GLWindow::init(int width, int height, const std::string& title)
{
    m_width = width;
    m_height = height;
    m_title = title;

    m_render_ctx->init(this);

    m_UI_ctx->init(this);

    m_scene_view = std::make_unique<SceneView>();

    m_property_pannel = std::make_unique<PropertyPanel>();

    m_property_pannel->set_mesh_load_callback(
      [this](std::string filepath) { m_scene_view->load_mesh(filepath); });

    return m_is_running;
}

GLWindow::~GLWindow()
{
    m_UI_ctx->end();

    m_render_ctx->end();
}

void GLWindow::on_resize(int width, int height)
{
    m_width = width;
    m_height = height;

    m_scene_view->resize(m_width, m_height);
    render();
}

void GLWindow::on_scroll(double delta)
{
    m_scene_view->on_mouse_wheel(delta);
}

void GLWindow::on_key(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
    }
}

void GLWindow::on_close()
{
    m_is_running = false;
}

void GLWindow::render()
{
    // Clear the view
    m_render_ctx->pre_render();

    // Initialize UI components
    m_UI_ctx->pre_render();

    // render scene to framebuffer and add it to scene view
    m_scene_view->render();

    m_property_pannel->render(m_scene_view.get());

    // Render the UI 
    m_UI_ctx->post_render();

    // Render end, swap buffers
    m_render_ctx->post_render();

    handle_input();
}

void GLWindow::handle_input()
{
    // TODO: move this and camera to scene UI component?

    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
    {
        m_scene_view->on_mouse_wheel(-0.4f);
    }

    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
    {
        m_scene_view->on_mouse_wheel(0.4f);
    }

    if (glfwGetKey(m_window, GLFW_KEY_F) == GLFW_PRESS)
    {
        m_scene_view->reset_view();
    }

    double x, y;
    glfwGetCursorPos(m_window, &x, &y);

    m_scene_view->on_mouse_move(x, y, WinInput::GetPressedButton(m_window));
}