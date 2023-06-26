#pragma once

#include "render/render_base.h"
#include "elems/vertex.h"
#include "elems/element.h"

#include <memory>   // required on windows

class Mesh : public Element
{
public:
    Mesh() = default;
  
    virtual ~Mesh();
  
    bool load(const std::string& filepath);
  
    void add_vertex(const Vertex& vertex) { m_vertices.push_back(vertex);  }
  
    void add_vertex_index(unsigned int vertex_idx) { m_vertex_indices.push_back(vertex_idx); }
  
    std::vector<unsigned int> get_vertex_indices() { return m_vertex_indices; }
  
    void update(Shader* shader) override { }
    
    glm::vec3 m_color = { 1.0f, 0.0f, 0.0f };
    float m_roughness = 0.2f;
    float m_metallic = 0.1f;
  
    void init();
  
    void create_buffers();
  
    void delete_buffers();
  
    void render(bool is_shade = true);
  
    void bind();
  
    void unbind();

private:
    // Buffers manager
    std::unique_ptr<VertexIndexBuffer> m_render_buffer_mgr;
    
    // Vertices and indices
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_vertex_indices;
};

class QuadMesh : public Element
{
public:
    QuadMesh() = default;
  
    virtual ~QuadMesh();
  
    void add_vertex(const Vertex& vertex) { m_vertices.push_back(vertex);  }
  
    void add_vertex_index(unsigned int vertex_idx) { m_vertex_indices.push_back(vertex_idx); }
  
    std::vector<unsigned int> get_vertex_indices() { return m_vertex_indices; }
  
    void update(Shader* shader) override {}
    
    void init();
  
    void create_buffers();
  
    void delete_buffers();
  
    void render();
  
    void bind();
  
    void unbind();

private:
    // Buffers manager
    std::unique_ptr<VertexIndexBuffer> m_render_buffer_mgr;
    
    // Vertices and indices
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_vertex_indices;
};
