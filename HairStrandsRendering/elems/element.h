#pragma once

#include "Shader.h"

class Element
{
public:
    virtual void update(Shader* shader) = 0;
};
