#version 450 core

#extension GL_EXT_gpu_shader4 : enable

uniform vec3 color;

uniform vec3 light_color_1;
uniform vec3 light_color_2;
uniform vec3 light_color_3;
uniform vec3 light_color_4;

uniform vec3 light_dir_1;
uniform vec3 light_dir_2;
uniform vec3 light_dir_3;
uniform vec3 light_dir_4;

uniform float Ka;
uniform float Kd;
uniform float Ks;

uniform int width;
uniform int height;

uniform sampler2D depth_map;
uniform sampler2D opacity_map;

uniform sampler2D depth_range_map;
uniform usampler2D occupancy_map;
uniform sampler2D slab_map;

uniform float alpha;
uniform float strands_shadow;
uniform float mesh_shadow;

in VSOUT
{
    float depth;

    vec4 light_view_position_1;
    vec4 light_view_position_2;
    vec4 light_view_position_3;
    vec4 light_view_position_4;
    
    float light_view_depth_1;
    float light_view_depth_2;
    float light_view_depth_3;
    float light_view_depth_4;

    vec3 color;
    vec3 tangent;
    vec3 view;

}fs_in;

out vec4 frag_color;

void GetKajiyaBRDF(out vec3 val1, out vec3 val2, out vec3 val3, out vec3 val4)
{
    float Shininess = 50.;
    vec3 Kcd = vec3(1., 1., 1.) * Kd;
    vec3 Kcs = vec3(1., 1., 1.) * Ks;
    vec3 t = normalize(fs_in.tangent);
    vec3 e = normalize(fs_in.view);
    float tcose = clamp(dot(t, e), -1., 1.);
    float tsine = sqrt(clamp(1. - tcose * tcose, 0., 1.));

    float tcosl1 = dot(t, light_dir_1);
    float tsinl1 = sqrt(clamp(1. - tcosl1 * tcosl1, 0., 1.));
    val1 = Kcd * tsinl1 + Kcs * pow((1. - tcose * tcosl1 + tsine * tsinl1) * 0.5, Shininess);

    float tcosl2 = dot(t, light_dir_2);
    float tsinl2 = sqrt(clamp(1. - tcosl2 * tcosl2, 0., 1.));
    val2 = Kcd * tsinl2 + Kcs * pow((1. - tcose * tcosl2 + tsine * tsinl2) * 0.5, Shininess);

    float tcosl3 = dot(t, light_dir_3);
    float tsinl3 = sqrt(clamp(1. - tcosl3 * tcosl3, 0., 1.));
    val3 = Kcd * tsinl3 + Kcs * pow((1. - tcose * tcosl3 + tsine * tsinl3) * 0.5, Shininess);

    float tcosl4 = dot(t, light_dir_4);
    float tsinl4 = sqrt(clamp(1. - tcosl4 * tcosl4, 0., 1.));
    val4 = Kcd * tsinl4 + Kcs * pow((1. - tcose * tcosl4 + tsine * tsinl4) * 0.5, Shininess);
}

void GetOpacityFactor(out float val1, out float val2, out float val3, out float val4)
{
    float tolerance = 0.00008;

    // 1
    vec4 lpos1 = fs_in.light_view_position_1 / fs_in.light_view_position_1.w;
    vec2 tex_pos1 = (lpos1.xy * 0.5 + vec2(0.5, 0.5)) * 0.5;
    val1 = 0.;
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 4; ++j){
            vec2 tex_pos = tex_pos1 + vec2((float(i) - 1.5) / 256., (float(j) - 1.5) / 256.);
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
            val1 += pow(1. - strands_shadow, mix(opacity_range_1.x, opacity_range_1.y, ratio1));
        }
    }
    float head_depth_1 = texture(depth_map, tex_pos1).y;
    val1 *= fs_in.light_view_depth_1 > head_depth_1 + tolerance ? 1. - mesh_shadow : 1.;
    val1 /= 16.;

    // 2
    vec4 lpos2 = fs_in.light_view_position_2 / fs_in.light_view_position_2.w;
    vec2 tex_pos2 = (lpos2.xy * 0.5 + vec2(0.5, 0.5)) * 0.5 + vec2(0.5, 0.);
    val2 = 0.;
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 4; ++j){
            vec2 tex_pos = tex_pos2 + vec2((float(i) - 1.5) / 256., (float(j) - 1.5) / 256.);
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
            val2 += pow(1.- strands_shadow, mix(opacity_range_2.x, opacity_range_2.y, ratio2));
        }
    }
    float head_depth_2 = texture(depth_map, tex_pos2).y;
    val2 *= fs_in.light_view_depth_2 > head_depth_2 + tolerance ? 1. - mesh_shadow : 1.;
    val2 /= 16.;

    // 3
    vec4 lpos3 = fs_in.light_view_position_3 / fs_in.light_view_position_3.w;
    vec2 tex_pos3 = (lpos3.xy * 0.5 + vec2(0.5, 0.5)) * 0.5 + vec2(0., 0.5);
    val3 = 0.;
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 4; ++j){
            vec2 tex_pos = tex_pos3 + vec2((float(i) - 1.5) / 256., (float(j) - 1.5) / 256.);
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
            val3 += pow(1. - strands_shadow, mix(opacity_range_3.x, opacity_range_3.y, ratio3));
        }
    }
    float head_depth_3 = texture(depth_map, tex_pos3).y;
    val3 *= fs_in.light_view_depth_3 > head_depth_3 + tolerance ? 1. - mesh_shadow : 1.;
    val3 /= 16.;

    // 4
    vec4 lpos4 = fs_in.light_view_position_4 / fs_in.light_view_position_4.w;
    vec2 tex_pos4 = (lpos4.xy * 0.5 + vec2(0.5, 0.5)) * 0.5 + vec2(0.5, 0.5);
    val4 = 0.;
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 4; ++j){
            vec2 tex_pos = tex_pos4 + vec2((float(i) - 1.5) / 256., (float(j) - 1.5) / 256.);
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
            val4 += pow(1.- strands_shadow, mix(opacity_range_4.x, opacity_range_4.y, ratio4));
        }
    }
    float head_depth_4 = texture2D(depth_map, tex_pos4).y;
    val4 *= fs_in.light_view_depth_4 > head_depth_4 + tolerance ? 1. - mesh_shadow : 1.;
    val4 /= 16.;
}

int bits_count(uint v)
{
        uint c = v - ((v >> 1u) & 0x55555555u);
        c = ((c >> 2u) & 0x33333333u) + (c & 0x33333333u);
        c = ((c >> 4u) + c) & 0x0f0f0f0fu;
        c = ((c >> 8u) + c) & 0x00ff00ffu;
        c = ((c >> 16u) + c) & 0x0000ffffu;
        return int(c);
}

float GetOccupancyFactor()
{
    // Compute the depth id
    vec2 tex_coord = gl_FragCoord.xy / vec2(float(width), float(height));
    vec2 range = texture(depth_range_map, tex_coord).xy;
    float ratio = sqrt((fs_in.depth - range.x) / max(-range.y - range.x, 1e-6));
    int depth_id = clamp(int(ratio * 128.), 0, 127);
    int slab_id = depth_id / 32;
    uint mask = ~(uint(0xffffffff) << uint(depth_id % 32));

    uvec4 occ = texture(occupancy_map, tex_coord);
    vec4 slab = texture(slab_map, tex_coord);
    uint occx;
    float slabx;
    float slab0;
    if(0 == slab_id){
        occx = occ.x;
        slabx = slab.x;
        slab0 = 0.;
    }
    else if(1 == slab_id){
        occx = occ.y;
        slabx = slab.y;
        slab0 = slab.x;
    }
    else if(2 == slab_id){
        occx = occ.z;
        slabx = slab.z;
        slab0 = slab.x + slab.y;
    }
    else{
        occx = occ.w;
        slabx = slab.w;
        slab0 = slab.x + slab.y + slab.z;
    }

    int total = bits_count(occx);
    int before = bits_count(occx & mask);
    float order = slab0 + slabx * float(before) / max(float(total), 1e-6);

    return pow((1. - alpha), order) * alpha;
}

void main()
{
    vec3 Kajiya1, Kajiya2, Kajiya3, Kajiya4;
    GetKajiyaBRDF(Kajiya1, Kajiya2, Kajiya3, Kajiya4);

    float opacity_factor_1, opacity_factor_2, opacity_factor_3, opacity_factor_4;
    GetOpacityFactor(opacity_factor_1, opacity_factor_2, opacity_factor_3, opacity_factor_4);

    float occupancy_factor = GetOccupancyFactor();

    vec3 main_color;

    if (color.x == 0 && color.y == 0 && color.z == 0)
    {
        main_color = fs_in.color;
    }
    else{
        main_color = color;
    }

    frag_color = vec4(main_color * ( // careful, the fs_in.color is a hack
                                     light_color_1 * Kajiya1 * opacity_factor_1 +
                                     light_color_2 * Kajiya2 * opacity_factor_2 +
                                     light_color_3 * Kajiya3 * opacity_factor_3 +
                                     light_color_4 * Kajiya4 * opacity_factor_4 +
                                     Ka), 1.) * occupancy_factor;
}