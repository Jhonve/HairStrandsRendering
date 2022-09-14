#version 330

in float Depth;

out vec4 OutColor;


void main()
{
    // outColor = vec4(gl_FragCoord.z, -gl_FragCoord.z, 0.0f, 0.0f);
    OutColor = vec4(Depth, -Depth, 0.0f, 0.0f);
}
