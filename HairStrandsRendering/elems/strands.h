#pragma once

#include "render/render_base.h"
#include "elems/element.h"
#include "elems/vertex.h"

#include <memory>   // required on windows

class Strands : public Element
{
public:
    typedef std::vector<std::vector<glm::vec3>> points;

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
  
    void render(bool is_shade = true, int width = 2);
  
    void bind();
  
    void unbind();

private:
    points load_bin(const std::string& filepath);
    points load_usc_data(const std::string& filepath);
    points try_simple_case();

private:
    // Buffers manager
    std::unique_ptr<StrandsIndexBuffer> m_render_buffer_mgr;
    
    // Vertices and indices
    int m_num_strands = 0;
    int m_num_points = 0;
    points m_points;

    std::vector<StrandVertex> m_strands_vertices;
    std::vector<unsigned int> m_vertex_indices;
};
