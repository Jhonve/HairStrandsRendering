#include "elems/strands.h"
#include "render/parameters.h"
#include "render/buffer_manager.h"

Strands::~Strands()
{
    delete_buffers();
}

void Strands::init(const Strands::points& points)
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
    int idx_points = 0;
    for (int i_strand = 0; i_strand < m_num_strands; i_strand++)
    {
        int num_points = m_original_points[i_strand].size();
        if (num_points < 3)
            continue;

        LaplaceSmoothing lap_smooth(4.0f, 2.0f);
        StrandVertex strand_vertex = m_strands_vertices[idx_points];
        idx_points += num_points;
    }

    // if (m_vertices.size() < 3)
    // {
    //     // LOG(ERROR) << "Data Size Is Less Than Three, No Need To Smooth";
    //     return;
    // }

    // zcg::Smoothing::LaplaceSmoothing ls(4.0f, 2.0f);
    // ls.build_coeff_mat(m_vertices.size());

    // std::vector<float> xarray(m_vertices.size());
    // std::vector<float> yarray(m_vertices.size());
    // std::vector<float> zarray(m_vertices.size());
    // for (int i = 0; i < m_vertices.size(); i++)
    // {
    //     StrandVertex& vert = m_vertices[i];
    //     xarray[i] = vert.position.x;
    //     yarray[i] = vert.position.y;
    //     zarray[i] = vert.position.z;
    // }

    // if (flag & 0x1)
    // {
    //     ls.Smooth(xarray);
    // }
    // if (flag & 0x2)
    // {
    //     ls.Smooth(yarray);
    // }
    // if (flag & 0x4)
    // {
    //     ls.Smooth(zarray);
    // }

    // for (int i = 1; i < m_vertices.size(); i++)
    // {
    //     StrandVertex& vert = m_vertices[i];
    //     vert.position.x = xarray[i];
    //     vert.position.y = yarray[i];
    //     vert.position.z = zarray[i];
    // }
}

void Strands::downsample()
{
    // int idx_points = 0;
    // int idx_count = 0;
    // for (int i_strand = 0; i_strand < m_num_strands; i_strand++)
    // {
    //     int num_points = m_original_points[i_strand].size();
    //     
    //     // add the 1st points
    //     StrandVertex strand_vertex_1 = m_strands_vertices[idx_points];
    //     m_downsampled_strands_vertices.push_back(strand_vertex_1);
    //     for (int j_point = 1; j_point < num_points - 1; j_point++)
    //     {
    //         StrandVertex strand_vertex_2 = m_strands_vertices[(size_t)idx_points + j_point];
    //         if (glm::dot(strand_vertex_1.m_tangent, strand_vertex_2.m_tangent) < m_downsample_sim_thres)
    //         {
    //             m_downsampled_strands_vertices.push_back(strand_vertex_2);
    //             m_downsampled_vertex_indices.push_back(idx_count);
    //             m_downsampled_vertex_indices.push_back(idx_count + 1);
    //             idx_count++;
    //         }
    //         strand_vertex_1 = strand_vertex_2;
    //     }
    //     idx_points += num_points;
    //     idx_count++;
    // }
}

void Strands::parametrical()
{
    
}

Strands::points Strands::load_bin(const std::string& filepath)
{
    points strands_points;
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
        std::vector<glm::vec3> strand_points(num_points, glm::vec3(0));
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
        }
    }
    fclose(f);
    return strands_points;
}

Strands::points Strands::load_usc_data(const std::string& filepath)
{
    points strands_points;
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
            std::vector<glm::vec3> strand_points(num_points, glm::vec3(0));
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
        smooth();
        // downsample();
        // printf("number of full %d, number of downsampled %d.\n number of strands %d.\n",
        //     m_strands_vertices.size(), m_downsampled_strands_vertices.size(), m_num_strands);
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