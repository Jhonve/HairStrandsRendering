#pragma once

#include <string>

class ImWindow
{
public:
    virtual void* get_native_window() = 0;
  
    virtual void set_native_window(void* window) = 0;
  
    virtual void on_scroll(double delta) = 0;
  
    virtual void on_key(int key, int scancode, int action, int mods) = 0;
  
    virtual void on_resize(int width, int height) = 0;
  
    virtual void on_close() = 0;
  
    int m_width;
    int m_height;
    std::string m_title;
};