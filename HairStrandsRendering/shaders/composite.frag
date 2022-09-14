#version 450 core

in VSOUT
{
    vec2 tex_coord;
}fs_in;

uniform sampler2D mesh_map;
uniform sampler2D strands_map;
uniform sampler2D slab_map;

uniform float alpha;
uniform float gamma;

out vec4 frag_out;

void main()
{
    vec4 mesh_color = texture(mesh_map, fs_in.tex_coord);
    vec4 strands_color = texture(strands_map, fs_in.tex_coord);
    vec4 slab = texture(slab_map, fs_in.tex_coord);
    
    vec4 frame_color = vec4(0.25, 0.25, 0.25, 1.0f);

    mesh_color.rgb = pow(mesh_color.rgb, vec3(gamma, gamma, gamma));

    vec3 mix_mesh_color = mix(frame_color.rgb, mesh_color.rgb, mesh_color.w);
    vec3 background_color = mix_mesh_color;

    float n = slab.x + slab.y + slab.z + slab.w;
    float back_alpha = pow(1. - alpha, n);
    background_color *= back_alpha;

    strands_color.rgb *= (1. - back_alpha) / max(strands_color.w, 1e-6);
    strands_color.rgb = pow(strands_color.rgb, vec3(gamma, gamma, gamma));

    frag_out = vec4((background_color + strands_color.rgb), 1.0f);
}