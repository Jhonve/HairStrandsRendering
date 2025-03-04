#version 450 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 tangent;
layout (location=2) in vec3 color;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

uniform vec3 cam_pos;

uniform mat4 light_view_mat_1;
uniform mat4 light_view_mat_2;
uniform mat4 light_view_mat_3;
uniform mat4 light_view_mat_4;

uniform mat4 light_view_proj_mat_1;
uniform mat4 light_view_proj_mat_2;
uniform mat4 light_view_proj_mat_3;
uniform mat4 light_view_proj_mat_4;

out VSOUT
{
    float depth;

    vec4 light_view_position_1;
    vec4 light_view_position_2;
    vec4 light_view_position_3;
    vec4 light_view_position_4;
    
    float light_view_depth_1;
    float light_view_depth_2;
    float light_view_depth_3;
    float light_view_depth_4;

    vec3 color;
    vec3 tangent;
    vec3 view;
}vs_out;

void main()
{
    gl_Position = proj_mat * view_mat * model_mat * vec4(position, 1.0f);
    vs_out.color = color;

    vs_out.view = normalize(cam_pos - position);
	vs_out.tangent = normalize(mat3(transpose(inverse(model_mat))) * tangent);

    vs_out.light_view_depth_1 = -(light_view_mat_1 * model_mat * vec4(position, 1.0f)).z;
    vs_out.light_view_depth_2 = -(light_view_mat_2 * model_mat * vec4(position, 1.0f)).z;
    vs_out.light_view_depth_3 = -(light_view_mat_3 * model_mat * vec4(position, 1.0f)).z;
    vs_out.light_view_depth_4 = -(light_view_mat_4 * model_mat * vec4(position, 1.0f)).z;

    vs_out.light_view_position_1 = light_view_proj_mat_1 * model_mat * vec4(position, 1.0f);
    vs_out.light_view_position_2 = light_view_proj_mat_2 * model_mat * vec4(position, 1.0f);
    vs_out.light_view_position_3 = light_view_proj_mat_3 * model_mat * vec4(position, 1.0f);
    vs_out.light_view_position_4 = light_view_proj_mat_4 * model_mat * vec4(position, 1.0f);

    vs_out.depth = -(view_mat * model_mat * vec4(position, 1.0f)).z;
}