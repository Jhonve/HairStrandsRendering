#include "elems/strands.h"
#include "render/parameters.h"
#include "render/buffer_manager.h"

Strands::~Strands()
{
    delete_buffers();
}

void Strands::init(const Strands::StrandsPoints& points)
{   
    m_strands_vertices.clear();
    m_vertex_indices.clear();
    int index_count = 0;
    for (int i_strand = 0; i_strand < m_num_strands; i_strand++)
    {
        int num_points = points[i_strand].size();
        
        // add point and tangent
        int random_color = k_strands_colors[i_strand % k_num_strands_colors];
        glm::vec3 color = glm::vec3((random_color >> 16) / 255.0f, ((random_color >> 8) % 256) / 255.0f, (random_color % 256) / 255.0f);

        for (int j_point = 0; j_point < num_points - 1; j_point++)
        {
            StrandVertex strand_vertex;
            strand_vertex.m_pos = points[i_strand][j_point] * 0.006f - glm::vec3(0.6f, 0.6f, 0.6f);   // TODO Shen
            strand_vertex.m_color = color;

            glm::vec3 tangent = points[i_strand][j_point + (size_t)1] -  points[i_strand][j_point];
            strand_vertex.m_tangent = glm::normalize(tangent);
            
            m_strands_vertices.push_back(strand_vertex);
            m_vertex_indices.push_back(index_count);
            m_vertex_indices.push_back(index_count + 1);
            index_count++;
        }

        StrandVertex strand_vertex;
        strand_vertex.m_pos = points[i_strand][num_points - (size_t)1] * 0.006f - glm::vec3(0.6f, 0.6f, 0.6f);    //  TODO Shen
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

void Strands::smooth()
{
    if (m_num_strands == 0)
        return;

    m_smoothed_points.clear();
    m_num_points = 0;

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

        LaplaceSmoothing lap_smooth(4.0f, 2.0f);
        lap_smooth.build_coeff_mat(num_points);
        lap_smooth.smooth(smoothed_strand);

        m_smoothed_points.push_back(smoothed_strand);
        m_num_points += smoothed_strand.size();
    }

    init(m_smoothed_points);
}

void Strands::downsample()
{
    if (m_smoothed_points.size() == 0)
        return;
    
    m_downsampled_points.clear();
    m_downsampled_tangents.clear();
    m_num_points = 0;

    for (int i_strand = 0; i_strand < m_num_strands; i_strand++)
    {
        int num_points = m_smoothed_points[i_strand].size();
        if (num_points < 3)
        {
            m_downsampled_points.push_back(m_smoothed_points[i_strand]);
            m_num_points += num_points;
            continue;
        }

        StrandPoints strand_points;
        StrandPoints strand_tangents;
        
        // add the 1st point
        strand_points.push_back(m_smoothed_points[i_strand][0]);
        glm::vec3 tangent_0 = glm::normalize(m_smoothed_points[i_strand][1] -
                                             m_smoothed_points[i_strand][0]);
        strand_tangents.push_back(tangent_0);
        for (int j_point = 1; j_point < num_points - 1; j_point++)
        {
            glm::vec3 point_1 = m_smoothed_points[i_strand][j_point];
            glm::vec3 point_2 = m_smoothed_points[i_strand][j_point + 1];

            glm::vec3 tangent_1 = glm::normalize(point_2 - point_1);

            if (glm::dot(tangent_1, tangent_0) < m_downsample_sim_thres)
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

        m_downsampled_points.push_back(strand_points);
        m_downsampled_tangents.push_back(strand_tangents);
    }

    init(m_downsampled_points);
}

void Strands::parametrical()
{
    if (m_downsampled_points.size() == 0)
        return;
    
    m_parametric_points.clear();
    m_num_points = 0;

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
                m_interp_points);

            strand_points.insert(strand_points.end(), interp_points.begin(), interp_points.end());
        }
        m_parametric_points.push_back(strand_points);
        m_num_points += strand_points.size();
    }

    init(m_parametric_points);
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
            }
            strands_points.push_back(strand_points);
            m_num_strands++;
            m_num_points += num_points;
        }
    }
    fclose(f);
    return strands_points;
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
    m_original_points.clear();
    m_num_points = 0;
    m_num_strands = 0;
    // check suffix
    if (filepath[filepath.length() - 1] == 'n') // .bin
        m_original_points = load_bin(filepath);
    else
        m_original_points = load_usc_data(filepath);
    
    if (m_num_strands > 0)
    {
        init(m_original_points);
        return true;
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