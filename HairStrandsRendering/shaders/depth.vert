#version 450 core

layout (location=0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

out VSOUT
{
    float depth;
}vs_out;

void main()
{
    gl_Position = proj_mat * view_mat * model_mat * vec4(position, 1.0f);
    vs_out.depth = -(view_mat * model_mat * vec4(position, 1.0f)).z;
}
