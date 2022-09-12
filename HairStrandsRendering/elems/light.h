#pragma once

#include "elems/element.h"

class Light : public Element
{
public:
    Light()
    {
      m_color = glm::vec3(1.0f, 1.0f, 1.0f);
      m_position = { 1.5f, 3.5f, 3.0f };
      m_strength = 100.0f;
    }

    ~Light() {}

    void update(Shader* shader) override
    {
      shader->set_vec3(m_position, "light_position");
      shader->set_vec3(m_color * m_strength, "light_color");
    }

    glm::vec3 m_position;

    glm::vec3 m_color;

    float m_strength;
};