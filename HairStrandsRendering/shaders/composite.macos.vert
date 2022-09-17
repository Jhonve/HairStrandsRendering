#version 150

in vec3 position;
in vec2 tex_coord;

out VSOUT
{
    vec2 tex_coord;
}vs_out;

void main()
{
	gl_Position = vec4(position, 1.0f);
    vs_out.tex_coord = tex_coord;
}