#pragma once

#include <vector>
#include <GL/glew.h>

class Face
{
private:
  std::vector<GLuint> m_vertex_idx;

public:

  void add_vertex_index(GLuint index) { m_vertex_idx.push_back(index); }

};