#pragma once

#include <vector>
#include <glm/glm.hpp>

class Vertex
{
public:
    Vertex() : m_pos(), m_normal() {}
  
    Vertex(const glm::vec3& pos, const glm::vec3& normal) : m_pos(pos), m_normal(normal)
    {
    }

    ~Vertex() = default;

    glm::vec3 m_pos;
    glm::vec3 m_normal;
    glm::vec2 m_uv;	// optional
};

class StrandVertex
{
public:
    StrandVertex() : m_pos(), m_tangent(), m_color() {}

    StrandVertex(const glm::vec3& points,
        const glm::vec3& tagents, 
        const glm::vec3& colors) : 
        m_pos(points), m_tangent(tagents), m_color(colors)
    {
    }

    ~StrandVertex() = default;

    glm::vec3 m_pos;
    glm::vec3 m_tangent;
    glm::vec3 m_color;
};
