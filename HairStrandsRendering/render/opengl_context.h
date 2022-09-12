#pragma once

#include "render_base.h"

class OpenGLContext : public RenderContext
{
public:

    bool init(ImWindow* window) override;

    void pre_render() override;

    void post_render() override;

    void end() override;
};