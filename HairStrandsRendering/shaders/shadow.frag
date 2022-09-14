#version 330
in float Depth;

uniform int Width;
uniform int Height;

uniform sampler2D DepthMap;

out vec4 OutColor;

void main()
{
    vec2 texPos = gl_FragCoord.xy / vec2(float(Width), float(Height));
    float depth0 = texture(DepthMap, texPos).x;

    float ratio = max(Depth - depth0, 0.) * 0.2;
    int depthID = int(ratio);
    if (depthID == 0)
        OutColor = vec4(1., 1., 1., 0.);
    else if (depthID == 1)
        OutColor = vec4(0., 1., 1., 0.);
    else
        OutColor = vec4(0., 0., 1., 0.);
}
