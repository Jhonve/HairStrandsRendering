#pragma once

#include "render/render_base.h"
#include "elems/element.h"
#include "elems/strand.h"

#include <memory>   // required on windows

class Strands : public Element
{
public:
    typedef std::vector<std::vector<glm::vec3>> points;
    typedef std::vector<std::vector<glm::vec3>> tagents;
    typedef std::vector<std::vector<glm::vec3>> colors;

public:
	Strands() = default;

	virtual ~Strands();

	bool load(const std::string& filepath);

    void update(Shader* shader) override
    {

    }
    
    void init();

    void create_buffers();
  
    void delete_buffers();
  
    void render();
  
    void bind();
  
    void unbind();

private:
    points load_bin(const std::string& filepath);
    points load_usc_data(const std::string& filepath);

private:
    // Buffers manager
    std::unique_ptr<StrandsIndexBuffer> m_render_buffer_mgr;
    
    // Vertices and indices
    std::vector<Strand> m_strands;
    points m_points;

    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_vertex_indices;
};
