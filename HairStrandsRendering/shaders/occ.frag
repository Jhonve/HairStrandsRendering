#version 330

in float Depth;

uniform int Width;
uniform int Height;

uniform sampler2D DepthRangeMap;

out uvec4 OccupancyBits;


void main()
{
    // Compute the depth id
    vec2 texcoord = gl_FragCoord.xy / vec2(float(Width), float(Height));
    vec2 range = texture(DepthRangeMap, texcoord).xy;
    float ratio = sqrt(max(Depth - range.x, 0.) / max(-range.y - range.x, 1e-6));
    int depthId = min(int(ratio * 128.), 127);
    int slabId = depthId / 32;
    int fragId = depthId % 32;
    int orValue = 1 << fragId;

    if (slabId == 0)
        OccupancyBits = uvec4(orValue, 0, 0, 0);
    else if (slabId == 1)
        OccupancyBits = uvec4(0, orValue, 0, 0);
    else if (slabId == 2)
        OccupancyBits = uvec4(0, 0, orValue, 0);
    else
        OccupancyBits = uvec4(0, 0, 0, orValue);

	// OccupancyBits = vec4(depthId, 0.0f, 0.0f, 1.0f);
}
