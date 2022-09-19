#version 450 core

in VSOUT
{
    float depth;
}fs_in;

out uvec4 occ_bits;

uniform int width;
uniform int height;

uniform sampler2D depth_range_map;

void main()
{
    // Compute the depth id
    vec2 texcoord = gl_FragCoord.xy / vec2(float(width), float(height));
    vec2 range = texture(depth_range_map, texcoord).xy;
    float ratio = sqrt(max(fs_in.depth - range.x, 0.) / max(-range.y - range.x, 1e-6));
    int depth_id = min(int(ratio * 128.), 127);
    uint slab_id = depth_id / 32;
    uint frag_id = depth_id % 32;
    uint value = 1 << frag_id;

    if (slab_id == 0)
        occ_bits = uvec4(value, 0, 0, 0);
    else if (slab_id == 1)
        occ_bits = uvec4(0, value, 0, 0);
    else if (slab_id == 2)
        occ_bits = uvec4(0, 0, value, 0);
    else
        occ_bits = uvec4(0, 0, 0, value);
}
