#include "elems/strands.h"
#include "render/parameters.h"
#include "render/buffer_manager.h"

#include <string>
#include <cmath>

Strands::~Strands()
{
    delete_buffers();
}

void Strands::init(const Strands::StrandsPoints& points)
{   
    m_strands_vertices.clear();
    m_vertex_indices.clear();
    int index_count = 0;
    
    glm::vec3 centric_pos = (m_original_min_pos + m_original_max_pos) / 2.f;
    float unit_scale = glm::length(m_original_max_pos - m_original_min_pos) / 2.f;
    float view_scale = (1.f / unit_scale) * 2.5f;
    // printf("centric_pos: %f %f, %f, unit_scale: %f, view_scale: %f\n", centric_pos.x, centric_pos.y, centric_pos.z, unit_scale, view_scale);
    for (int i_strand = 0; i_strand < m_num_strands; i_strand++)
    {
        int num_points = points[i_strand].size();
        
        // add point and tangent
        glm::vec3 color;
        if (m_strands_color)
            color = m_strands_color->at(i_strand);
        else
        {
            int random_color = k_strands_colors[i_strand % k_num_strands_colors];
            color = glm::vec3((random_color >> 16) / 255.0f, ((random_color >> 8) % 256) / 255.0f, (random_color % 256) / 255.0f);
        }

        for (int j_point = 0; j_point < num_points - 1; j_point++)
        {
            StrandVertex strand_vertex;
            strand_vertex.m_pos = (points[i_strand][j_point] - centric_pos) * view_scale;
            strand_vertex.m_color = color;

            glm::vec3 tangent = points[i_strand][j_point + (size_t)1] - points[i_strand][j_point];
            strand_vertex.m_tangent = glm::normalize(tangent);
            
            m_strands_vertices.push_back(strand_vertex);
            m_vertex_indices.push_back(index_count);
            m_vertex_indices.push_back(index_count + 1);
            index_count++;
        }

        StrandVertex strand_vertex;
        strand_vertex.m_pos = (points[i_strand][num_points - (size_t)1] - centric_pos) * view_scale;
        strand_vertex.m_color = color;
        strand_vertex.m_tangent = m_strands_vertices[m_strands_vertices.size() - 1].m_tangent;
        m_strands_vertices.push_back(strand_vertex);
        index_count++;
    }
    
    // init buffer
    if (!m_render_buffer_mgr)
        m_render_buffer_mgr = std::make_unique<OpenGLStrandsIndexBuffer>();
    else
        m_render_buffer_mgr->delete_buffers();
    create_buffers();
}

void Strands::smooth(float lap_const, float pos_const)
{
    if (m_num_strands == 0)
        return;

    m_smoothed_points.clear();
    m_num_points = 0;
    
    m_num_strands = m_original_points.size();

    for (int i_strand = 0; i_strand < m_num_strands; i_strand++)
    {
        int num_points = m_original_points[i_strand].size();
        StrandPoints smoothed_strand = m_original_points[i_strand];
        
        if (num_points < 3)
        {
            m_smoothed_points.push_back(smoothed_strand);
            m_num_points += smoothed_strand.size();
            continue;
        }

        LaplaceSmoothing lap_smooth(lap_const, pos_const);
        lap_smooth.build_coeff_mat(num_points);
        lap_smooth.smooth(smoothed_strand);

        m_smoothed_points.push_back(smoothed_strand);
        m_num_points += smoothed_strand.size();
    }

    init(m_smoothed_points);
}

void Strands::downsample(float ds_sim_thres, int min_num_pts)
{
    if (m_smoothed_points.size() == 0)
        return;
    
    m_downsampled_points.clear();
    m_downsampled_tangents.clear();
    m_num_points = 0;

    m_num_strands = m_smoothed_points.size();

    for (int i_strand = 0; i_strand < m_num_strands; i_strand++)
    {
        int num_points = m_smoothed_points[i_strand].size();
        if (num_points <= 2)
            continue;
        else if (num_points > 2 && num_points < min_num_pts)
        {
            StrandPoints strand_points;
            StrandPoints strand_tangents;
            
            glm::vec3 tangent = glm::normalize(m_smoothed_points[i_strand][1] -
                                                 m_smoothed_points[i_strand][0]);
            for (int j_point = 0; j_point < num_points - 1; j_point++)
            {
                glm::vec3 point_0 = m_smoothed_points[i_strand][j_point];
                glm::vec3 point_2 = m_smoothed_points[i_strand][j_point + 1];

                glm::vec3 point_1 = (point_0 + point_2) / 2.f;

                tangent = glm::normalize(point_2 - point_0);

                strand_points.push_back(m_smoothed_points[i_strand][j_point]);
                strand_tangents.push_back(tangent);
                strand_points.push_back(point_1);
                strand_tangents.push_back(tangent);
                m_num_points += 2;
            }
            // add the last point
            strand_points.push_back(m_smoothed_points[i_strand][num_points - 1]);
            strand_tangents.push_back(tangent);

            m_downsampled_points.push_back(strand_points);
            m_downsampled_tangents.push_back(strand_tangents);
        }
        else
        {
            StrandPoints strand_points;
            StrandPoints strand_tangents;

            // add the 1st point
            strand_points.push_back(m_smoothed_points[i_strand][0]);
            glm::vec3 tangent_0 = glm::normalize(m_smoothed_points[i_strand][1] -
                m_smoothed_points[i_strand][0]);
            strand_tangents.push_back(tangent_0);
            m_num_points++;

            int num_segs = std::floor((double)(num_points - 2) / (double)(min_num_pts -2));

            for (int j_point = 1; j_point < num_points - 1; j_point++)
            {
                glm::vec3 point_1 = m_smoothed_points[i_strand][j_point];
                glm::vec3 point_2 = m_smoothed_points[i_strand][j_point + 1];

                glm::vec3 tangent_1 = glm::normalize(point_2 - point_1);

                if (glm::dot(tangent_1, tangent_0) < ds_sim_thres || j_point % num_segs == 0)
                {
                    strand_points.push_back(m_smoothed_points[i_strand][j_point]);
                    tangent_0 = tangent_1;
                    strand_tangents.push_back(tangent_0);
                    m_num_points++;
                }
            }
            // add the last point
            strand_points.push_back(m_smoothed_points[i_strand][num_points - 1]);
            strand_tangents.push_back(tangent_0);
            m_num_points++;

            m_downsampled_points.push_back(strand_points);
            m_downsampled_tangents.push_back(strand_tangents);
        }
    }
    
    m_num_strands = m_downsampled_points.size();
    init(m_downsampled_points);
}

void Strands::parametrical(int num_interp_pts)
{
    if (m_downsampled_points.size() == 0)
        return;
    
    m_parametric_points.clear();
    m_num_points = 0;

    m_num_strands = m_downsampled_points.size();

    for (int i_strand = 0; i_strand < m_num_strands; i_strand++)
    {
        int num_points = m_downsampled_points[i_strand].size();

        StrandPoints strand_points;

        for (int j_point = 0; j_point < num_points - 1; j_point++)
        {
            StrandPoints interp_points = Hermit_spline(
                m_downsampled_points[i_strand][j_point],
                m_downsampled_tangents[i_strand][j_point],
                m_downsampled_points[i_strand][j_point + 1],
                m_downsampled_tangents[i_strand][j_point + 1],
                num_interp_pts);

            strand_points.insert(strand_points.end(), interp_points.begin(), interp_points.end());
        }
        m_parametric_points.push_back(strand_points);
        m_num_points += strand_points.size();
    }

    init(m_parametric_points);
}

void Strands::duplicate(int dup_ratio, float dup_perturbation)
{
    if (m_downsampled_points.size() == 0)
        return;
    
    m_duplicated_points.clear();
    m_num_points = 0;

    for (int i_strand = 0; i_strand < m_num_strands; i_strand++)
    {
        int num_points = m_downsampled_points[i_strand].size();
        StrandPoints strand_points = m_downsampled_points[i_strand];
        m_duplicated_points.push_back(strand_points);

        glm::vec3 start_tangent = m_downsampled_tangents[i_strand][0];
        for (int j_sam = 0; j_sam < dup_ratio; j_sam++)
        {
            float rand_x = (float)rand() / RAND_MAX;
            float rand_y = (float)rand() / RAND_MAX;

            float orgo_z = -(rand_x * start_tangent.x + rand_y * start_tangent.y) / start_tangent.z;

            float scale_ration =(float)rand() / RAND_MAX * dup_perturbation + dup_perturbation; 
            glm::vec3 rand_offset = glm::normalize(glm::vec3(rand_x, rand_y, orgo_z)) * scale_ration;
            
            StrandPoints dup_strand;
            for (int k_point = 0; k_point < num_points; k_point++)
                dup_strand.push_back(strand_points[k_point] + rand_offset);

            m_duplicated_points.push_back(dup_strand);
        }
        m_num_points += (dup_ratio + 1) * num_points;
    }

    m_num_strands = m_duplicated_points.size();
    init(m_duplicated_points);
}

Strands::StrandsPoints Strands::load_bin(const std::string& filepath)
{
    StrandsPoints strands_points;
#ifdef _WIN32
    FILE* f; 
    fopen_s(&f, filepath.c_str(), "rb");
#else
    FILE* f = fopen(filepath.c_str(), "rb");
#endif
    if (!f)
    {
        fprintf(stderr, "Couldn't open %s\n", filepath.c_str());
        return strands_points;
    }

    int num_strands = 0;
    fread(&num_strands, 4, 1, f);
    // printf("num_strands: %d\n", num_strands);
    for (int i_strand = 0; i_strand < num_strands; i_strand++)
    {
        int num_points = 0;
        fread(&num_points, 4, 1, f);
        StrandPoints strand_points(num_points, glm::vec3(0));
        float dummy = 0.0f;
        for (int j_point = 0; j_point < num_points; j_point++)
        {
            fread(&strand_points[j_point].x, 4, 1, f);
            fread(&strand_points[j_point].y, 4, 1, f);
            fread(&strand_points[j_point].z, 4, 1, f);
            fread(&dummy, 4, 1, f); // nx unused
            fread(&dummy, 4, 1, f); // ny unused
            fread(&dummy, 4, 1, f); // nz unused
            fread(&dummy, 4, 1, f); // label unused

            m_original_min_pos = glm::min(m_original_min_pos, strand_points[j_point]);
            m_original_max_pos = glm::max(m_original_max_pos, strand_points[j_point]);
        }
        bool valid_strand = true;
        for (int j_point = 0; j_point < num_points - 1; j_point++)
        {
            if (glm::length(strand_points[j_point] - strand_points[j_point + (size_t)1]) < 1.e-6)
            {
                valid_strand = false;
                break;
            }
        }
        if (valid_strand)
        {
            strands_points.push_back(strand_points);
            m_num_strands++;
            m_num_points += num_points;
        }
    }
    fclose(f);
    return strands_points;
}

Strands::StrandsPoints Strands::load_cin(const std::string& filepath)
{
    StrandsPoints strands_points;
    m_strands_color = std::make_shared<std::vector<glm::vec3>>();

#ifdef _WIN32
    FILE* f; 
    fopen_s(&f, filepath.c_str(), "rb");
#else
    FILE* f = fopen(filepath.c_str(), "rb");
#endif
    if (!f)
    {
        fprintf(stderr, "Couldn't open %s\n", filepath.c_str());
        return strands_points;
    }

    int num_strands = 0;
    fread(&num_strands, 4, 1, f);
    for (int i_strand = 0; i_strand < num_strands; i_strand++)
    {
        int num_points = 0;
        fread(&num_points, 4, 1, f);
        StrandPoints strand_points(num_points, glm::vec3(0));
        glm::vec3 strand_color;
        float dummy = 0.0f;
        for (int j_point = 0; j_point < num_points; j_point++)
        {
            fread(&strand_points[j_point].x, 4, 1, f);
            fread(&strand_points[j_point].y, 4, 1, f);
            fread(&strand_points[j_point].z, 4, 1, f);
            fread(&strand_color.x, 4, 1, f);
            fread(&strand_color.y, 4, 1, f);
            fread(&strand_color.z, 4, 1, f);
            fread(&dummy, 4, 1, f); // label unused

            m_original_min_pos = glm::min(m_original_min_pos, strand_points[j_point]);
            m_original_max_pos = glm::max(m_original_max_pos, strand_points[j_point]);
        }
        bool valid_strand = true;
        for (int j_point = 0; j_point < num_points - 1; j_point++)
        {
            if (glm::length(strand_points[j_point] - strand_points[j_point + (size_t)1]) < 1.e-4)
            {
                valid_strand = false;
                break;
            }
        }
        if (valid_strand)
        {
            strands_points.push_back(strand_points);
            m_strands_color->push_back(strand_color);
            m_num_strands++;
            m_num_points += num_points;
        }
    }
    fclose(f);
    return strands_points;
}

Strands::StrandsPoints Strands::load_usc_data(const std::string& filepath)
{
    StrandsPoints strands_points;
#ifdef _WIN32
    FILE* f; 
    fopen_s(&f, filepath.c_str(), "rb");
#else
    FILE* f = fopen(filepath.c_str(), "rb");
#endif
    if (!f)
    {
        fprintf(stderr, "Couldn't open %s\n", filepath.c_str());
        return strands_points;
    }

    int num_strands = 0;
    fread(&num_strands, 4, 1, f);
    for (int i_strand = 0; i_strand < num_strands; i_strand++)
    {
        int num_points;
        fread(&num_points, 4, 1, f);
        if (num_points == 1)
        {
            glm::vec3 dummy;
            fread(&dummy.x, 4, 1, f);
            fread(&dummy.y, 4, 1, f);
            fread(&dummy.z, 4, 1, f);
        }
        else
        {
            StrandPoints strand_points(num_points, glm::vec3(0));
            for (int j_point = 0; j_point < num_points; j_point++)
            {
                fread(&strand_points[j_point].x, 4, 1, f);
                fread(&strand_points[j_point].y, 4, 1, f);
                fread(&strand_points[j_point].z, 4, 1, f);

                m_original_min_pos = glm::min(m_original_min_pos, strand_points[j_point]);
                m_original_max_pos = glm::max(m_original_max_pos, strand_points[j_point]);
            }
            strands_points.push_back(strand_points);
            m_num_strands++;
            m_num_points += num_points;
        }
    }
    fclose(f);
    return strands_points;
}

Strands::StrandsPoints Strands::load_hair(const std::string& filepath)
{
    StrandsPoints strands_points;
#ifdef _WIN32
    FILE* f; 
    fopen_s(&f, filepath.c_str(), "rb");
#else
    FILE* f = fopen(filepath.c_str(), "rb");
#endif
    if (!f)
    {
        fprintf(stderr, "Couldn't open %s\n", filepath.c_str());
        return strands_points;
    }
    
    char header[4];
    fread(&header, 4, 1, f);
    if (header[0] == 'H' && header[1] == 'A' && header[2] == 'I' && header[3] == 'R')
    {
        // fprintf(stderr, "Read Cem Yuksel's hairstyles\n");   // http://www.cemyuksel.com/research/hairmodels/
        // refer to https://github.com/cemyuksel/cyCodeBase/blob/master/cyHairFile.h

        unsigned int cy_hair_file_segments_bit = 1;
        unsigned int cy_hair_file_points_bit = 2;

        unsigned int num_strands = 0;
        unsigned int points_count = 0;
        fread(&num_strands, sizeof(unsigned int), 1, f);
        fread(&points_count, sizeof(unsigned int), 1, f);

        unsigned int bit_array;
        fread(&bit_array, sizeof(unsigned int), 1, f);

        if (bit_array < 0 || !(bit_array & cy_hair_file_points_bit))
        {
            fprintf(stderr, "Error: cyHairFile::LoadFromFile - Invalid file.\n");
            return strands_points;
        }
        
        unsigned int default_num_seg = 0;
        if (!(bit_array & cy_hair_file_segments_bit))
        {
            fread(&default_num_seg, sizeof(unsigned int), 1, f);
        }
        
        rewind(f);
        unsigned int dummy_header[128];
        fread(&dummy_header, 128, 1, f);

        unsigned short* segments = new unsigned short[num_strands];
        float* points = new float[points_count * 3];
        if (default_num_seg == 0)
        {
            fread(segments, sizeof(unsigned short) * num_strands, 1, f);
            fread(points, sizeof(float) * points_count * 3, 1, f);
        }
        else
        {
            for (int i_strand = 0; i_strand < num_strands; i_strand++)
                segments[i_strand] = default_num_seg;
            fread(points, sizeof(float) * points_count * 3, 1, f);
        }

        for (int i_strand = 0; i_strand < num_strands; i_strand++)
        {
            int num_points = segments[i_strand] + 1;
            StrandPoints strand_points(num_points, glm::vec3(0));
            for (int j_point = 0; j_point < num_points; j_point++)
            {
                strand_points[j_point].x = points[m_num_points * 3 + j_point * 3];
                strand_points[j_point].y = points[m_num_points * 3 + j_point * 3 + 1];
                strand_points[j_point].z = points[m_num_points * 3 + j_point * 3 + 2];

                m_original_min_pos = glm::min(m_original_min_pos, strand_points[j_point]);
                m_original_max_pos = glm::max(m_original_max_pos, strand_points[j_point]);
            }
            strands_points.push_back(strand_points);
            m_num_strands++;
            m_num_points += num_points;
        }

        return strands_points;
    }
    else
    {
        // zju hair format

        rewind(f);

        unsigned int num_strands = 0;
        unsigned int points_count = 0;
        fread(&num_strands, sizeof(unsigned int), 1, f);
        fread(&points_count, sizeof(unsigned int), 1, f);

        unsigned short* segments = new unsigned short[num_strands];
        float* points = new float[points_count * 3];
        fread(segments, sizeof(unsigned short) * num_strands, 1, f);
        fread(points, sizeof(float) * points_count * 3, 1, f);
        fclose(f);

        for (int i_strand = 0; i_strand < num_strands; i_strand++)
        {
            int num_points = segments[i_strand];
            StrandPoints strand_points(num_points, glm::vec3(0));
            for (int j_point = 0; j_point < num_points; j_point++)
            {
                strand_points[j_point].x = points[m_num_points * 3 + j_point * 3];
                strand_points[j_point].y = points[m_num_points * 3 + j_point * 3 + 1];
                strand_points[j_point].z = points[m_num_points * 3 + j_point * 3 + 2];

                m_original_min_pos = glm::min(m_original_min_pos, strand_points[j_point]);
                m_original_max_pos = glm::max(m_original_max_pos, strand_points[j_point]);
            }
            strands_points.push_back(strand_points);
            m_num_strands++;
            m_num_points += num_points;
        }

        return strands_points;
    }
}


bool Strands::save_bin(const std::string& filepath, const StrandsPoints& strands_points)
{
#ifdef _WIN32
    FILE* f; 
    fopen_s(&f, filepath.c_str(), "wb");
#else
    FILE* f = fopen(filepath.c_str(), "wb");
#endif
    if (!f)
    {
        fprintf(stderr, "Couldn't open %s\n", filepath.c_str());
        return false;
    }

    int currentVertex = 0;
    
    fwrite(&m_num_strands, sizeof(int), 1, f);
    for (unsigned int i = 0; i < m_num_strands; i++) {
        int num_points = strands_points[i].size();
        fwrite(&num_points, sizeof(int), 1, f);
        // For each strand, first read all of the vertices
        for (unsigned int j = 0; j < num_points; j++) {
            glm::vec3 pt = strands_points[i][j];
            fwrite(&pt.x, sizeof(float), 1, f);
            fwrite(&pt.y, sizeof(float), 1, f);
            fwrite(&pt.z, sizeof(float), 1, f);
            fwrite(&pt.x, sizeof(float), 1, f);
            fwrite(&pt.y, sizeof(float), 1, f);
            fwrite(&pt.z, sizeof(float), 1, f);
            fwrite(&pt.z, sizeof(uint32_t), 1, f);
        }
    }
    fclose(f);
    return true;
}

bool Strands::save_usc_data(const std::string& filepath, const StrandsPoints& strands_points)
{
#ifdef _WIN32
    FILE* f; 
    fopen_s(&f, filepath.c_str(), "wb");
#else
    FILE* f = fopen(filepath.c_str(), "wb");
#endif
    if (!f)
    {
        fprintf(stderr, "Couldn't open %s\n", filepath.c_str());
        return false;
    }

    fwrite(&m_num_strands, 4, 1, f);
    for (int i = 0; i < m_num_strands; i++)
    {
        int num_points = strands_points[i].size();
        fwrite(&num_points, 4, 1, f);
        for (int j = 0; j < num_points; j++)
        {
            glm::vec3 pt = strands_points[i][j];
            fwrite(&pt.x, 4, 1, f);
            fwrite(&pt.y, 4, 1, f);
            fwrite(&pt.z, 4, 1, f);
        }
    }
    fclose(f);
    return true;
}

Strands::StrandPoints Strands::Hermit_spline(const glm::vec3 begin_pos, const glm::vec3 begin_tangent,
                                             const glm::vec3 end_pos, const glm::vec3 end_tangent, const int num_iter)
{
    StrandPoints interp_points(num_iter);

    const float interp_scale = 1.0f / num_iter;
    const glm::vec3 diff_pos = end_pos - begin_pos;
    float diff_len = glm::length(diff_pos);
    const glm::vec3 diff_normal = glm::normalize(diff_pos);

    for (int i = 0; i < num_iter; i++)
    {
        float t = interp_scale * i;

        interp_points[i] = ((2.0f * powf(t, 3.0f) - 3.0f * powf(t, 2.0f) + 1.0f) * glm::vec3(0.0f)
            + (powf(t, 3.0f) - 2.0f * powf(t, 2.0f) + t) * begin_tangent
            + (-2.0f * powf(t, 3.0f) + 3.0f * powf(t, 2.0f)) * diff_normal
            + (powf(t, 3.0f) - powf(t, 2.0f)) * end_tangent) * diff_len + begin_pos;
    }
    return interp_points;
}

bool Strands::load(const std::string& filepath)
{
    m_filepath = filepath;
    m_original_points.clear();
    m_original_min_pos = glm::vec3(FLT_MAX);
    m_original_max_pos = glm::vec3(FLT_MIN);
    m_num_points = 0;
    m_num_strands = 0;
    // check suffix
    if (filepath[filepath.length() - 3] == 'b') // .bin
        m_original_points = load_bin(filepath);
    else if (filepath[filepath.length() - 3] == 'c')
        m_original_points = load_cin(filepath);
    else if (filepath[filepath.length() - 4] == 'd')
        m_original_points = load_usc_data(filepath);
    else
        m_original_points = load_hair(filepath);
    
    if (m_num_strands > 0)
    {
        init(m_original_points);
        return true;
    }
    return false;
}

bool Strands::save(std::string mode)
{
    std::string out_filepath = m_filepath;
    
    if (mode == "smoothed")
    {
        if (m_smoothed_points.size() == 0)
            return false;

        if (out_filepath[out_filepath.length() - 1] == 'n')
        {
            out_filepath.insert(out_filepath.length() - 4, "_smoothed");
            return save_bin(out_filepath, m_smoothed_points);
        }
        else
        {
            out_filepath.insert(out_filepath.length() - 5, "_smoothed");
            return save_usc_data(out_filepath, m_smoothed_points);
        }
    }
    else if (mode == "downsampled")
    {
        if (m_downsampled_points.size() == 0)
            return false;

        if (out_filepath[out_filepath.length() - 1] == 'n')
        {
            out_filepath.insert(out_filepath.length() - 4, "_downsampled");
            return save_bin(out_filepath, m_downsampled_points);
        }
        else
        {
            out_filepath.insert(out_filepath.length() - 5, "_downsampled");
            return save_usc_data(out_filepath, m_downsampled_points);
        }
    }
    else if (mode == "duplicated")
    {
        if (m_duplicated_points.size() == 0)
            return false;

        if (out_filepath[out_filepath.length() - 1] == 'n')
        {
            out_filepath.insert(out_filepath.length() - 4, "_duplicated");
            return save_bin(out_filepath, m_duplicated_points);
        }
        else
        {
            out_filepath.insert(out_filepath.length() - 5, "_duplicated");
            return save_usc_data(out_filepath, m_duplicated_points);
        }
    }
    else if (mode == ".bin")
    {
        if (m_original_points.size() == 0)
            return false;

        if (out_filepath[out_filepath.length() - 1] == 'n')
            out_filepath.replace(out_filepath.length() - 4, out_filepath.length() - 1, ".bin");
        else
            out_filepath.replace(out_filepath.length() - 5, out_filepath.length() - 1, ".bin");

        printf("%s\n", out_filepath.c_str());
        return save_bin(out_filepath, m_original_points);
    }
    else if (mode == ".data")
    {
        if (m_original_points.size() == 0)
            return false;

        if (out_filepath[out_filepath.length() - 1] == 'n')
            out_filepath.replace(out_filepath.length() - 4, out_filepath.length() - 1, ".data");
        else
            out_filepath.replace(out_filepath.length() - 5, out_filepath.length() - 1, ".data");

        printf("%s\n", out_filepath.c_str());
        return save_usc_data(out_filepath, m_original_points);
    }

    return false;
}

void Strands::create_buffers()
{
    m_render_buffer_mgr->create_buffers(m_strands_vertices, m_vertex_indices);
}
  
void Strands::delete_buffers()
{
    m_render_buffer_mgr->delete_buffers();
}
  
void Strands::bind()
{
    m_render_buffer_mgr->bind();
}
  
void Strands::unbind()
{
    m_render_buffer_mgr->unbind();
}
  
void Strands::render(bool is_shade, int width)
{
    m_render_buffer_mgr->draw((int) m_vertex_indices.size(), is_shade, width);
}
