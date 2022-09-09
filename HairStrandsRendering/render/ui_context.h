#pragma once

#include "render_base.h"

namespace nrender
{
  class UIContext : public RenderContext
  {

  public:

    bool init(nwindow::IWindow* window) override;

    void pre_render() override;

    void post_render() override;

    void end() override;

  };
}
