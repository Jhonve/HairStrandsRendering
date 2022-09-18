#include "ui/opengl_context.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

static void on_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto pWindow = static_cast<ImWindow*>(glfwGetWindowUserPointer(window));
    pWindow->on_key(key, scancode, action, mods);
}

static void on_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto pWindow = static_cast<ImWindow*>(glfwGetWindowUserPointer(window));
    pWindow->on_scroll(yoffset);
}

static void on_window_size_callback(GLFWwindow* window, int width, int height)
{
    auto pWindow = static_cast<ImWindow*>(glfwGetWindowUserPointer(window));
    pWindow->on_resize(width, height);
}

static void on_window_close_callback(GLFWwindow* window)
{
    ImWindow* pWindow = static_cast<ImWindow*>(glfwGetWindowUserPointer(window));
    pWindow->on_close();
}

bool OpenGLContext::init(ImWindow* window)
{
#ifdef _WIN32
    __super::init(window);
#else
    m_window = window;
#endif

    /* Initialize the library */
    if (!glfwInit())
    {
        fprintf(stderr, "Error: GLFW Window couldn't be initialized\n");
        return false;
    }

// before create glfwwindow
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create the window and store this window as window pointer
    // so that we can use it in callback functions
    auto glWindow = glfwCreateWindow(window->m_width, window->m_height, window->m_title.c_str(), nullptr, nullptr);
    window->set_native_window(glWindow);

    if (!glWindow)
    {
        fprintf(stderr, "Error: GLFW Window couldn't be created\n");
        return false;
    }

    glfwSetWindowUserPointer(glWindow, window);
    glfwSetKeyCallback(glWindow, on_key_callback);
    glfwSetScrollCallback(glWindow, on_scroll_callback);
    glfwSetWindowSizeCallback(glWindow, on_window_size_callback);
    glfwSetWindowCloseCallback(glWindow, on_window_close_callback);
    glfwMakeContextCurrent(glWindow);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf( "Failed to initialize OpenGL context");
        return -1;
    }

    return true;
}

void OpenGLContext::pre_render()
{
    glViewport(0, 0, m_window->m_width, m_window->m_height);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLContext::post_render()
{
    glfwPollEvents();
    glfwSwapBuffers((GLFWwindow*) m_window->get_native_window());
}

void OpenGLContext::end()
{
    glfwDestroyWindow((GLFWwindow*)m_window->get_native_window());
    glfwTerminate();
}