#pragma once

#include "window.h"

class ImWindowContext
{

public:
    ImWindowContext() : m_window(nullptr) {}

    virtual bool init(ImWindow* window)
    {
        m_window = window;
        return true;
    }

    virtual void pre_render() = 0;

    virtual void post_render() = 0;

    virtual void end() = 0;

protected:
    ImWindow* m_window;
};