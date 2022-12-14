#if __INTELLISENSE__
#undef __ARM_NEON
#undef __ARM_NEON__
#endif
// Above is needed on macos vscode to decrease incorrcet errors when include eigen
// refer to https://github.com/microsoft/vscode-cpptools/issues/7413

#pragma once

#include "render/render_base.h"
#include "elems/element.h"
#include "elems/vertex.h"

#include <memory>   // required on windows
#include <Eigen/Sparse>
#include <Eigen/Dense>

class Strands : public Element
{
public:
    typedef std::vector<std::vector<glm::vec3>> StrandsPoints;
    typedef std::vector<glm::vec3> StrandPoints;
    std::shared_ptr<std::vector<glm::vec3>> m_strands_color;

public:
    Strands() : m_num_strands(0) {}

    virtual ~Strands();

    bool load(const std::string& filepath);
    bool save(std::string mode);

    void init(const StrandsPoints& points);
    void smooth(float lap_const = 4.0f, float pos_const = 2.0f);
    void downsample(float ds_sim_thres = 0.98f, int min_num_pts = 5);
    void parametrical(int num_interp_pts = 10);
    void duplicate(int dup_ratio = 10, float dup_perturbation = 1.0f);

    void update(Shader* shader) override {}

    void create_buffers();
    void delete_buffers();
    void render(bool is_shade = true, int width = 2);
  
    void bind();
    void unbind();

    int get_num_strands() { return m_num_strands; }
    int get_num_points() { return m_num_points; }

private:
    StrandsPoints load_bin(const std::string& filepath);
    StrandsPoints load_cin(const std::string& filepath);   // load color strands
    StrandsPoints load_usc_data(const std::string& filepath);
    bool save_bin(const std::string& filepath, const StrandsPoints& strands_points);
    bool save_usc_data(const std::string& filepath, const StrandsPoints& strands_points);

    StrandPoints Hermit_spline(const glm::vec3 begin_pos, const glm::vec3 begin_tangent,
                               const glm::vec3 end_pos, const glm::vec3 end_tangent, const int num_iter);

private:
    std::string m_filepath;
    // Buffers manager
    std::unique_ptr<StrandsIndexBuffer> m_render_buffer_mgr;
    
    // Vertices and indices
    int m_num_strands = 0;
    int m_num_points = 0;
    StrandsPoints m_original_points;
    StrandsPoints m_smoothed_points;
    StrandsPoints m_downsampled_points;
    StrandsPoints m_downsampled_tangents;   // should preserve original tangent
    StrandsPoints m_parametric_points;
    StrandsPoints m_duplicated_points;

    std::vector<StrandVertex> m_strands_vertices;
    std::vector<unsigned int> m_vertex_indices;
};


class LaplaceSmoothing
{
public:
    LaplaceSmoothing(float lap_constraint = 1.0f, float position_constraint = 1.0f)
        : m_lapl_constraint(lap_constraint), m_position_constraint(position_constraint)
    {};
    
    ~LaplaceSmoothing() {};
    
    void build_coeff_mat(int ndata)
    {
        int ntriple = 3 * ndata - 2 + ndata;
        std::vector<Eigen::Triplet<float>> coeff_triples(ntriple);
        
        int triple_count = -1;
        for (int n = 0; n < ndata; n++)
        {
            float coeff = 0.0f;
            coeff += (n > 0) ? 1.0f : 0.0f;
            coeff += (n < ndata - 1) ? 1.0f : 0.0f;
        
            if (n > 0)
            {
                triple_count++;
                coeff_triples[triple_count] = Eigen::Triplet<float>(n, n - 1, -1.0f * m_lapl_constraint);
            }
        
            triple_count++;
            coeff_triples[triple_count] = Eigen::Triplet<float>(n, n, coeff * m_lapl_constraint);
        
            if (n < ndata - 1)
            {
                triple_count++;
                coeff_triples[triple_count] = Eigen::Triplet<float>(n, n + 1, -1.0f * m_lapl_constraint);
            }
        }
        
        for (int n = 0; n < ndata; n++)
        {
            triple_count++;
            coeff_triples[triple_count] = Eigen::Triplet<float>(ndata + n, n, m_position_constraint);
        }
        
        Eigen::SparseMatrix<float> coeffMat(ndata + ndata, ndata);
        coeffMat.setFromTriplets(coeff_triples.begin(), coeff_triples.end());
        
        m_coeff_mat_T = coeffMat.transpose();
        m_coeff_mat_mult = m_coeff_mat_T * coeffMat;
        m_solver.compute(m_coeff_mat_mult);
    }
    
    void smooth(std::vector<glm::vec3>& curve, int begin = -1, int end = -1)
    {
        begin = begin == -1 ? 0 : begin;
        end = end == -1 ? curve.size() : end;
    
        std::vector<float> xarray(curve.size());
        std::vector<float> yarray(curve.size());
        std::vector<float> zarray(curve.size());
        for (int i = 0; i < curve.size(); i++)
        {
            xarray[i] = curve[i].x;
            yarray[i] = curve[i].y;
            zarray[i] = curve[i].z;
        }
    
        this->smooth(xarray, begin, end);
        this->smooth(yarray, begin, end);
        this->smooth(zarray, begin, end);

        for (int i = 0; i < curve.size(); i++)
        {
            curve[i].x = xarray[i];
            curve[i].y = yarray[i];
            curve[i].z = zarray[i];
        }
    }
    
    void smooth(std::vector<float>& curve, int begin = -1, int end = -1)
    {
        int ndata = curve.size();
        begin = begin == -1 ? 0 : begin;
        end = end == -1 ? ndata : end;
    
        Eigen::VectorXf bVec(ndata + ndata);
        for (int i = 0; i < ndata; i++)
        {
            bVec[i] = 0;
        }
        for (int i = 0; i < ndata; i++)
        {
            bVec[ndata + i] = m_position_constraint * curve[i];
        }
    
        Eigen::VectorXf xxVec(ndata);
        Eigen::VectorXf bVecT = m_coeff_mat_T * bVec;
        xxVec = m_solver.solve(bVecT);
    
        for (int i = begin; i < end; i++)
        {
            curve[i] = xxVec[i];
        }
    }

private:
    Eigen::SparseMatrix<float> m_coeff_mat_mult;
    Eigen::SparseMatrix<float> m_coeff_mat_T;
    Eigen::SimplicialLLT<Eigen::SparseMatrix<float>> m_solver;

    float m_lapl_constraint;
    float m_position_constraint;
};