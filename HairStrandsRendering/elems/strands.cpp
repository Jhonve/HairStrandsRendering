#include "elems/strands.h"
#include "render/opengl_buffer_manager.h"

void Strands::init()
{
    m_render_buffer_mgr = std::make_unique<OpenGLStrandsIndexBuffer>();
  
    // TODO
    create_buffers();
}

Strands::~Strands()
{
    delete_buffers();
}

Strands::points Strands::load_bin(const std::string& filepath)
{
    points strands_points;
    FILE* f; 
    fopen_s(&f, filepath.c_str(), "rb");
    if (!f)
    {
        fprintf(stderr, "Couldn't open %s\n", filepath);
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
            if (glm::length(strand_points[j_point] - strand_points[j_point + 1]) < 1.e-4)
            {
                valid_strand = false;
                break;
            }
        }
        if (valid_strand)
        {
            strands_points.push_back(strand_points);
        }
    }
    fclose(f);
}

Strands::points Strands::load_usc_data(const std::string& filepath)
{
    points strands_points;
    FILE* f;
    fopen_s(&f, filepath.c_str(), "rb");
    if (!f)
    {
        fprintf(stderr, "Couldn't open %s\n", filepath);
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
        }
    }
    fclose(f);

}

bool Strands::load(const std::string& filepath)
{
	// check suffix
	if (filepath[filepath.length() - 1] == 'n') // .bin
		load_bin(filepath);
	else
		load_usc_data(filepath);
}

void Strands::create_buffers()
{
    // TODO
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
  
void Strands::render()
{
    // TODO
}