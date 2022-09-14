#version 450 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

uniform vec3 view_point;

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
    vec4 light_view_position_1;
    vec4 light_view_position_2;
    vec4 light_view_position_3;
    vec4 light_view_position_4;
    
    float light_view_depth_1;
    float light_view_depth_2;
    float light_view_depth_3;
    float light_view_depth_4;

    vec3 normal;
    vec3 view;
}vs_out;

void main()
{
    gl_Position = proj_mat * view_mat * model_mat * vec4(position, 1.0f);
	vs_out.view = normalize(view_point - position);
    vs_out.normal = normalize(mat3(transpose(inverse(model_mat))) * normal);

    vs_out.light_view_depth_1 = -(light_view_mat_1 * model_mat * vec4(position, 1.0f)).z;
    vs_out.light_view_depth_2 = -(light_view_mat_2 * model_mat * vec4(position, 1.0f)).z;
    vs_out.light_view_depth_3 = -(light_view_mat_3 * model_mat * vec4(position, 1.0f)).z;
    vs_out.light_view_depth_4 = -(light_view_mat_4 * model_mat * vec4(position, 1.0f)).z;

    vs_out.light_view_position_1 = light_view_proj_mat_1 * model_mat * vec4(position, 1.0f);
    vs_out.light_view_position_2 = light_view_proj_mat_2 * model_mat * vec4(position, 1.0f);
    vs_out.light_view_position_3 = light_view_proj_mat_3 * model_mat * vec4(position, 1.0f);
    vs_out.light_view_position_4 = light_view_proj_mat_4 * model_mat * vec4(position, 1.0f);
}
