#version 450 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 color;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

uniform vec3 com_pos;

out VSOUT
{
    vec3 color;
}vs_out;

void main()
{
    gl_Position = proj_mat * view_mat * model_mat * vec4(position, 1.0f);
    vs_out.color = color;
}
