#version 450 core

in VSOUT
{
    float depth;
}fs_in;

uniform int width;
uniform int height;

uniform sampler2D depth_map;

out vec4 frag_color;

void main()
{
    vec2 texcoord = gl_FragCoord.xy / vec2(float(width), float(height));
    float depth_0 = texture(depth_map, texcoord).x;

    float ratio = max(fs_in.depth - depth_0, 0.) * 0.2;
    int depth_id = int(ratio);
    if (depth_id == 0)
        frag_color = vec4(1., 1., 1., 0.);
    else if (depth_id == 1)
        frag_color = vec4(0., 1., 1., 0.);
    else
        frag_color = vec4(0., 0., 1., 0.);
}
