#version 330

in float Depth;

out vec4 OutColor;

void main()
{
    OutColor = vec4(Depth, Depth, Depth, Depth);
}
