#version 450 core

in VSOUT
{
	vec3 world_pos;
	vec3 normal;
	vec3 color;
}fs_in;

uniform vec3 a_light_pos;
uniform vec3 b_light_pos;
uniform vec3 view_pos;

out vec4 frag_color;

float computeDiff(vec3 normal, vec3 light_dir);
float computeSpec(vec3 light_dir, vec3 view_dir);

void main()
{
	float ambient_weight = 0.3;
	float specular_weight = 0.3;
	vec3 light_color = vec3(1.0f, 1.0f, 1.0f);

	vec3 ambient = ambient_weight * light_color;

	vec3 a_light_dir = normalize(a_light_pos - fs_in.world_pos);
	vec3 b_light_dir = normalize(b_light_pos - fs_in.world_pos);
	vec3 view_dir = normalize(view_pos - fs_in.world_pos);

	// diff
	vec3 normal = normalize(fs_in.normal);
	float diff = computeDiff(normal, a_light_dir) + computeDiff(normal, b_light_dir);
	vec3 diffuse = diff * light_color;

	// specular  Blinn-Phong
	float spec = computeSpec(a_light_dir, view_dir) + computeSpec(b_light_dir, view_dir);
	vec3 specular = spec * specular_weight * light_color;

	vec3 total = ambient + diffuse + specular;

	// fragColor = vec4(vec3(1.0, 1.0f, 1.0f) * total, 1.0f);
	frag_color = vec4(fs_in.color * total, 1.0f);
}

float computeDiff(vec3 normal, vec3 light_dir)
{
	return max(dot(light_dir, normal), 0.0f);
}

float computeSpec(vec3 light_dir, vec3 view_dir)
{
	vec3 halfway_dir = normalize(light_dir + view_dir);
	return pow(max(dot(view_dir, halfway_dir), 0.0f), 16.0f);
}