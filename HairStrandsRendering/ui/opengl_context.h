#pragma once

#include "ui/context.h"

// OpenGLContext is designed for interacitve callback mouse/key events
class OpenGLContext : public ImWindowContext
{
public:
    bool init(ImWindow* window) override;

    void pre_render() override;

    void post_render() override;

    void end() override;
};