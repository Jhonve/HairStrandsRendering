#version 450 core

in VSOUT
{
    vec2 tex_coord;
}fs_in;

uniform sampler2D face_map;
uniform sampler2D strands_map;

uniform float alpha;
uniform float gamma;

out vec4 frag_out;

void main()
{
	vec4 face_color = texture(face_map, fs_in.tex_coord);
	vec4 hair_color = texture(strands_map, fs_in.tex_coord);
	
	frag_out = vec4(face_color.rgb + hair_color.rgb, 1.0f);
}