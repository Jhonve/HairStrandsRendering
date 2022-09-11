#pragma once

#include "render/shader.h"

class Element
{
public:
    virtual void update(Shader* shader) = 0;
};
