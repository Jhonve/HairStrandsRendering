#version 450 core

in VSOUT
{
    vec3 world_pos;
    vec3 normal;
    vec3 color;
}fs_in;

struct DirLight
{
    vec3 position;
    vec3 color;
    float strength;
};

uniform mat4 model;

uniform DirLight dir_light;

out vec4 frag_color;

void main ()  
{
    mat3 normal_matrix = transpose(inverse(mat3(model)));
    vec3 normal = normalize(normal_matrix * fs_in.normal);
    vec3 light_diff = dir_light.position - vec3(model * vec4(fs_in.world_pos, 1));
    float luminance = dot(normal, light_diff) / (length(light_diff) * length(normal));

    frag_color = vec4(luminance * dir_light.color, 1.0f) * dir_light.strength;
}