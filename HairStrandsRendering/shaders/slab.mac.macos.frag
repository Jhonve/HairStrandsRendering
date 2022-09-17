#version 150

in VSOUT
{
    float depth;
}fs_in;

uniform int width;
uniform int height;

uniform sampler2D depth_range_map;

out vec4 frag_color;

void main()
{
    vec2 range = texture(depth_range_map, gl_FragCoord.xy / vec2(float(width), float(height))).xy;
    float ratio = sqrt(max(fs_in.depth - range.x, 0.) / max(-range.y - range.x, 1e-6));
    int depth_id = min(int(ratio * 128.), 127);
    int slab_id = depth_id / 32;

    if (slab_id == 0)
        frag_color = vec4(1., 0., 0., 0.);
    else if (slab_id == 1)
        frag_color = vec4(0., 1., 0., 0.);
    else if (slab_id == 2)
        frag_color = vec4(0., 0., 1., 0.);
    else
        frag_color = vec4(0., 0., 0., 1.);
}
