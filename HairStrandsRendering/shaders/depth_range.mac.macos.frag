#version 150

in VSOUT
{
    float depth;
}fs_in;

out vec4 frag_out;

void main()
{
    // after opengl pipeline including z-buffer, for each pixel, the value is (depth_min, -depth_max)
    frag_out = vec4(fs_in.depth, -fs_in.depth, 0.0f, 0.0f);
}
