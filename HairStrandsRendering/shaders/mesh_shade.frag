#version 450 core

uniform vec3 light_color_1;
uniform vec3 light_color_2;
uniform vec3 light_color_3;
uniform vec3 light_color_4;

uniform vec3 light_dir_1;
uniform vec3 light_dir_2;
uniform vec3 light_dir_3;
uniform vec3 light_dir_4;

uniform sampler2D depth_map;
uniform sampler2D opacity_map;

uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform vec3 color;

uniform float strands_shadow;
uniform float mesh_shadow;

in VSOUT
{
    vec4 light_view_position_1;
    vec4 light_view_position_2;
    vec4 light_view_position_3;
    vec4 light_view_position_4;
    
    float light_view_depth_1;
    float light_view_depth_2;
    float light_view_depth_3;
    float light_view_depth_4;

    vec3 normal;
    vec3 view;
}fs_in;

out vec4 frag_color;

void GetShading(out vec3 val1, out vec3 val2, out vec3 val3, out vec3 val4)
{
	float Shininess = 2.;
	vec3 Kcd = Kd * color;
	vec3 Kcs = Ks * vec3(1., 1., 1.);
	vec3 n = normalize(fs_in.normal);
	vec3 e = normalize(fs_in.view);

	val1 = max(dot(light_dir_1, n), 0.) * Kcd + pow(max(dot(reflect(-light_dir_1, n), e), 0.), Shininess) * Kcs + vec3(Ka, Ka, Ka);
	val2 = max(dot(light_dir_2, n), 0.) * Kcd + pow(max(dot(reflect(-light_dir_2, n), e), 0.), Shininess) * Kcs + vec3(Ka, Ka, Ka);
	val3 = max(dot(light_dir_3, n), 0.) * Kcd + pow(max(dot(reflect(-light_dir_3, n), e), 0.), Shininess) * Kcs + vec3(Ka, Ka, Ka);
	val4 = max(dot(light_dir_4, n), 0.) * Kcd + pow(max(dot(reflect(-light_dir_4, n), e), 0.), Shininess) * Kcs + vec3(Ka, Ka, Ka);
}

void GetOpacityFactor(out float val1, out float val2, out float val3, out float val4)
{
	float tolerance = 0.02;

	// 1
    vec4 lpos1 = fs_in.light_view_position_1 / fs_in.light_view_position_1.w;
	vec2 tex_pos1 = (lpos1.xy * 0.5 + vec2(0.5, 0.5)) * 0.5;
	val1 = 0.;
	for (int i = 0; i < 6; ++i){
		for (int j = 0; j < 6; ++j){
			vec2 tex_pos = tex_pos1 + vec2((float(i) - 2.5) / 512., (float(j) - 2.5) / 512.);
            vec4 opacity1 = texture(opacity_map, tex_pos);
            float ratio1 = max(fs_in.light_view_depth_1 - opacity1.w, 0.) * 0.2;
			int depth_id_1 = min(int(ratio1), 2);
			ratio1 = min(ratio1 - float(depth_id_1), 1.);
			vec2 opacity_range_1;
			if (depth_id_1 == 0)
				opacity_range_1 = vec2(0., opacity1.x);
			else if (depth_id_1 == 1)
				opacity_range_1 = opacity1.xy;
			else
				opacity_range_1 = opacity1.yz;
			val1 += fs_in.light_view_depth_1 > texture(depth_map, tex_pos).y + tolerance ? (1. - strands_shadow) * (1. - mesh_shadow) : pow(1. - strands_shadow, mix(opacity_range_1.x, opacity_range_1.y, ratio1));
		}
	}
	val1 /= 36.;

	// 2
    vec4 lpos2 = fs_in.light_view_position_2 / fs_in.light_view_position_2.w;
	vec2 tex_pos2 = (lpos2.xy * 0.5 + vec2(0.5, 0.5)) * 0.5 + vec2(0.5, 0.);
	val2 = 0.;
	for (int i = 0; i < 6; ++i){
		for (int j = 0; j < 6; ++j){
			vec2 tex_pos = tex_pos2 + vec2((float(i) - 2.5) / 512., (float(j) - 2.5) / 512.);
            vec4 opacity2 = texture(opacity_map, tex_pos);
            float ratio2 = max(fs_in.light_view_depth_2 - opacity2.w, 0.) * 0.2;
			int depth_id_2 = min(int(ratio2), 2);
			ratio2 = min(ratio2 - float(depth_id_2), 1.);
			vec2 opacity_range_2;
			if (depth_id_2 == 0)
				opacity_range_2 = vec2(0., opacity2.x);
			else if (depth_id_2 == 1)
				opacity_range_2 = opacity2.xy;
			else
				opacity_range_2 = opacity2.yz;
			val2 += fs_in.light_view_depth_2 > texture(depth_map, tex_pos).y + tolerance ? (1. - strands_shadow) * (1. - mesh_shadow) : pow(1. - strands_shadow, mix(opacity_range_2.x, opacity_range_2.y, ratio2));
		}
	}
	val2 /= 36.;

	// 3
    vec4 lpos3 = fs_in.light_view_position_3 / fs_in.light_view_position_3.w;
	vec2 tex_pos3 = (lpos3.xy * 0.5 + vec2(0.5, 0.5)) * 0.5 + vec2(0., 0.5);
	val3 = 0.;
	for (int i = 0; i < 6; ++i){
		for (int j = 0; j < 6; ++j){
			vec2 tex_pos = tex_pos3 + vec2((float(i) - 2.5) / 512., (float(j) - 2.5) / 512.);
            vec4 opacity3 = texture(opacity_map, tex_pos);
            float ratio3 = max(fs_in.light_view_depth_3 - opacity3.w, 0.) * 0.2;
			int depth_id_3 = min(int(ratio3), 2);
			ratio3 = min(ratio3 - float(depth_id_3), 1.);
			vec2 opacity_range_3;
			if (depth_id_3 == 0)
				opacity_range_3 = vec2(0., opacity3.x);
			else if (depth_id_3 == 1)
				opacity_range_3 = opacity3.xy;
			else
				opacity_range_3 = opacity3.yz;
			val3 += fs_in.light_view_depth_3 > texture(depth_map, tex_pos).y + tolerance ? (1. - strands_shadow) * (1. - mesh_shadow) : pow(1. - strands_shadow, mix(opacity_range_3.x, opacity_range_3.y, ratio3));
		}
	}
	val3 /= 36.;

	// 4
    vec4 lpos4 = fs_in.light_view_position_4 / fs_in.light_view_position_4.w;
	vec2 tex_pos4 = (lpos4.xy * 0.5 + vec2(0.5, 0.5)) * 0.5 + vec2(0.5, 0.5);
	val4 = 0.;
	for (int i = 0; i < 6; ++i){
		for (int j = 0; j < 6; ++j){
			vec2 tex_pos = tex_pos4 + vec2((float(i) - 2.5) / 512., (float(j) - 2.5) / 512.);
            vec4 opacity4 = texture(opacity_map, tex_pos);
            float ratio4 = max(fs_in.light_view_depth_4 - opacity4.w, 0.) * 0.2;
			int depth_id_4 = min(int(ratio4), 2);
			ratio4 = min(ratio4 - float(depth_id_4), 1.);
			vec2 opacity_range_4;
			if (depth_id_4 == 0)
				opacity_range_4 = vec2(0., opacity4.x);
			else if (depth_id_4 == 1)
				opacity_range_4 = opacity4.xy;
			else
				opacity_range_4 = opacity4.yz;
			val4 += fs_in.light_view_depth_4 > texture(depth_map, tex_pos).y + tolerance ? (1. - strands_shadow) * (1. - mesh_shadow) : pow(1. - strands_shadow, mix(opacity_range_4.x, opacity_range_4.y, ratio4));
		}
	}
	val4 /= 36.;
}

void main()
{
	vec3 shading1, shading2, shading3, shading4;
	GetShading(shading1, shading2, shading3, shading4);

    float opacity_factor_1, opacity_factor_2, opacity_factor_3, opacity_factor_4;
    GetOpacityFactor(opacity_factor_1, opacity_factor_2, opacity_factor_3, opacity_factor_4);

    frag_color = vec4((
		light_color_1 * shading1 * opacity_factor_1 +
		light_color_2 * shading2 * opacity_factor_2 +
		light_color_3 * shading3 * opacity_factor_3 +
		light_color_4 * shading4 * opacity_factor_4 ), 1.);
}
