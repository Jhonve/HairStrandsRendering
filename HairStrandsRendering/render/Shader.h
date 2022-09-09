#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

// Utitlity class to load, compile and attach a vertex- and fragment shader to a program
class Shader
{

private:
    unsigned int m_program_id;

    unsigned int get_compiled_shader(unsigned int shader_type, const std::string& shader_source);

public:
    Shader() = default;

    // Load a vertex and a fragment shader from file
    bool load(const std::string& vert_shader_file, const std::string& frag_shader_file);

    // Use the program
    void use();

    // Delete the program
    void unload();

    unsigned int get_program_id() { return m_program_id; }

    void set_mat4(const glm::mat4& mat4, const std::string& name);

    void set_i1(int v, const std::string& name);
    void set_f1(float v, const std::string& name);
    void set_f3(float a, float b, float c, const std::string& name);

    void set_vec3(const glm::vec3& vec3, const std::string& name);
    void set_vec4(const glm::vec4& vec4, const std::string& name);
};
