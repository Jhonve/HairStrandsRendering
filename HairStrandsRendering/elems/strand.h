#pragma once

#include <vector>
#include <glm/glm.hpp>

class Strand
{
public:
	Strand() : m_points(), m_tagents(), m_colors() {}

	Strand(const std::vector<glm::vec3>& points,
		const std::vector<glm::vec3>& tagents, 
		const std::vector<glm::vec3>& colors) : 
		m_points(points), m_tagents(tagents), m_colors(colors)
	{

	}

	~Strand() = default;

	std::vector<glm::vec3> m_points;
	std::vector<glm::vec3> m_tagents;
	std::vector<glm::vec3> m_colors;
};
