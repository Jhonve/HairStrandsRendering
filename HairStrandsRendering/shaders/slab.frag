#version 330
in float Depth;

uniform int Width;
uniform int Height;

uniform sampler2D DepthRangeMap;

out vec4 outColor;

void main()
{
    // Compute the depth id
    vec2 range = texture(DepthRangeMap, gl_FragCoord.xy / vec2(float(Width), float(Height))).xy;
    float ratio = sqrt(max(Depth - range.x, 0.) / max(-range.y - range.x, 1e-6));
    int depthId = min(int(ratio * 128.), 127);
    int slabId = depthId / 32;

    if (slabId == 0)
        outColor = vec4(1., 0., 0., 0.);
    else if (slabId == 1)
        outColor = vec4(0., 1., 0., 0.);
    else if (slabId == 2)
        outColor = vec4(0., 0., 1., 0.);
    else
        outColor = vec4(0., 0., 0., 1.);
}
