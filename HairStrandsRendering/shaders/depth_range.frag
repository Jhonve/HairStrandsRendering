#version 450 core

in VSOUT
{
    float depth;
}fs_in;

out vec4 frag_out;

void main()
{
    frag_out = vec4(fs_in.depth, -fs_in.depth, 0.0f, 0.0f);
}
