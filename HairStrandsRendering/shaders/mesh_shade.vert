#version 330

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform vec3 ViewPoint;

uniform mat4 LightViewMatrix1;
uniform mat4 LightViewMatrix2;
uniform mat4 LightViewMatrix3;
uniform mat4 LightViewMatrix4;

uniform mat4 LightViewProjMatrix1;
uniform mat4 LightViewProjMatrix2;
uniform mat4 LightViewProjMatrix3;
uniform mat4 LightViewProjMatrix4;

out vec4 LightViewPosition1;
out vec4 LightViewPosition2;
out vec4 LightViewPosition3;
out vec4 LightViewPosition4;

out float LightViewDepth1;
out float LightViewDepth2;
out float LightViewDepth3;
out float LightViewDepth4;

out vec3 Normal;
out vec3 View;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1.0f);
	View = normalize(ViewPoint - position);
    Normal = normalize(mat3(transpose(inverse(ModelMatrix))) * normal);
	// Normal = normalize(normal);

	LightViewDepth1 = -(LightViewMatrix1 * ModelMatrix * vec4(position, 1.0f)).z;
	LightViewDepth2 = -(LightViewMatrix2 * ModelMatrix * vec4(position, 1.0f)).z;
	LightViewDepth3 = -(LightViewMatrix3 * ModelMatrix * vec4(position, 1.0f)).z;
	LightViewDepth4 = -(LightViewMatrix4 * ModelMatrix * vec4(position, 1.0f)).z;

	LightViewPosition1 = LightViewProjMatrix1 * ModelMatrix * vec4(position, 1.0f);
	LightViewPosition2 = LightViewProjMatrix2 * ModelMatrix * vec4(position, 1.0f);
	LightViewPosition3 = LightViewProjMatrix3 * ModelMatrix * vec4(position, 1.0f);
	LightViewPosition4 = LightViewProjMatrix4 * ModelMatrix * vec4(position, 1.0f);
}
