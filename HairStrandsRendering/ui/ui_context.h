#pragma once

#include "ui/context.h"

class UIContext : public ImWindowContext
{
public:
    bool init(ImWindow* window) override;

    void pre_render() override;

    void post_render() override;

    void end() override;

};