#include "render/shader.h"

#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

unsigned int Shader::get_compiled_shader(unsigned int shader_type, const std::string& shader_source)
{
    unsigned int shader_id = glCreateShader(shader_type);

    const char* c_source = shader_source.c_str();
    glShaderSource(shader_id, 1, &c_source, nullptr);
    glCompileShader(shader_id);

    GLint result;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

        GLchar* strInfoLog = new GLchar[length + 1];
        glGetShaderInfoLog(shader_id, length, &length, strInfoLog);

        fprintf(stderr, "Compile error in shader: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

    return shader_id;
}

bool Shader::load(const std::string& vertexshader_file, const std::string& fragmentshader_file)
{
    std::ifstream is_vs(vertexshader_file);
    const std::string f_vs((std::istreambuf_iterator<char>(is_vs)), std::istreambuf_iterator<char>());

    std::ifstream is_fs(fragmentshader_file);
    const std::string f_fs((std::istreambuf_iterator<char>(is_fs)), std::istreambuf_iterator<char>());

    m_program_id = glCreateProgram();

    unsigned int vs = get_compiled_shader(GL_VERTEX_SHADER, f_vs);
    unsigned int fs = get_compiled_shader(GL_FRAGMENT_SHADER, f_fs);

    glAttachShader(m_program_id, vs);
    glAttachShader(m_program_id, fs);

    glLinkProgram(m_program_id);
    glValidateProgram(m_program_id);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return true;
}

void Shader::use()
{
    glUseProgram(m_program_id);
}

void Shader::unload()
{
    glDeleteProgram(m_program_id);
}

void Shader::set_mat4(const glm::mat4& mat4, const std::string& name)
{
    GLint param_loc = glGetUniformLocation(get_program_id(), name.c_str());
    glUniformMatrix4fv(param_loc, 1, GL_FALSE, glm::value_ptr(mat4));
}

void Shader::set_i1(int v, const std::string& name)
{
    GLint param_loc = glGetUniformLocation(get_program_id(), name.c_str());
    glUniform1i(param_loc, v);
}

void Shader::set_f1(float v, const std::string& name)
{
    GLint param_loc = glGetUniformLocation(get_program_id(), name.c_str());
    glUniform1f(param_loc, v);
}

void Shader::set_f3(float a, float b, float c, const std::string& name)
{
    GLint param_loc = glGetUniformLocation(get_program_id(), name.c_str());
    glUniform3f(param_loc, a, b, c);
}

#if defined(__APPLE__)
void Shader::set_vec3(const glm::vec3& vec3, const std::string& name)
{
    GLint param_loc = glGetUniformLocation(get_program_id(), name.c_str());
    glUniform3f(param_loc, vec3.x, vec3.y, vec3.z);
}

void Shader::set_vec4(const glm::vec4& vec4, const std::string& name)
{
    GLint param_loc = glGetUniformLocation(get_program_id(), name.c_str());
    glUniform4f(param_loc, vec4.w, vec4.x, vec4.y, vec4.z);
}
#else
void Shader::set_vec3(const glm::vec3& vec3, const std::string& name)
{
    GLint param_loc = glGetUniformLocation(get_program_id(), name.c_str());
    glProgramUniform3fv(get_program_id(), param_loc, 1, glm::value_ptr(vec3));
}

void Shader::set_vec4(const glm::vec4& vec4, const std::string& name)
{
    GLint param_loc = glGetUniformLocation(get_program_id(), name.c_str());
    glProgramUniform4fv(get_program_id(), param_loc, 1, glm::value_ptr(vec4));
}
#endif