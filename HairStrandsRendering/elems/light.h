#pragma once

#include "elems/element.h"
#include "render/parameters.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Light : public Element
{
public:
    Light()
    {
      m_color = glm::vec3(1.0f, 1.0f, 1.0f);
      m_position = { 1.5f, 3.5f, 3.0f };
      m_strength = 100.0f;
    }

    ~Light() {}

    void update(Shader* shader) override
    {
      shader->set_vec3(m_position, "light_position");
      shader->set_vec3(m_color * m_strength, "light_color");
    }

public:
    glm::vec3 m_position;
    glm::vec3 m_color;
    float m_strength;
};

class Lights : public Element
{
public:
    Lights(float strength)
    {
        m_strength = strength;
        OriginalLightsParameters ori_lights_param;
        m_num_lights = ori_lights_param.num_lights;
        int num_param = ori_lights_param.num_param;

        for (int i_light = 0; i_light < m_num_lights; i_light++)
        {
            glm::vec3 color(ori_lights_param.lights[i_light * num_param],
                            ori_lights_param.lights[i_light * num_param + 1],
                            ori_lights_param.lights[i_light * num_param + 2]);
            glm::vec3 dir(ori_lights_param.lights[i_light * num_param + 3],
                          ori_lights_param.lights[i_light * num_param + 4],
                          ori_lights_param.lights[i_light * num_param + 5]);
            float intensity = ori_lights_param.lights[i_light * num_param + 6];
            
            color = color * intensity;
            m_ori_colors.push_back(color);
            m_ori_dirs.push_back(dir);
        }
        m_colors.resize(m_num_lights);
        m_dirs.resize(m_num_lights);

        init();
        get_lights_mat(glm::vec3(-1.f, -1.f, -1.f), glm::vec3(1.f, 1.f, 1.f)); // TODO
    }
    ~Lights()
    {
        m_ori_colors.clear();
        m_ori_dirs.clear();
        m_colors.clear();
        m_dirs.clear();
        m_view_mat.clear();
        m_proj_mat.clear();
        m_view_proj_mat.clear();
    }

    void init()
    {
        glm::vec3 from(0.0f, 1.0f, 0.0f);
        glm::vec3 to(0.0f, 0.5f, 1.0f);
        to = glm::normalize(to);

        glm::vec3 axis = glm::cross(from, to);
        float w = sqrt(glm::length(from) * glm::length(from) * glm::length(to) * glm::length(to)) + glm::dot(from, to);
        glm::quat q(w, axis);
        q = glm::normalize(q);

        glm::mat3 rot_mat = glm::toMat3(q); // forget about why we need this.

        for (int i_light = 0; i_light < m_num_lights; ++i_light)
        {
            m_colors[i_light] = m_ori_colors[i_light] * m_strength;
            m_dirs[i_light] = rot_mat * m_ori_dirs[i_light];
        }
    }

    void get_lights_mat(const glm::vec3 scene_min, const glm::vec3 scene_max)
    {
        m_view_mat.clear();
        m_proj_mat.clear();
        m_view_proj_mat.clear();

        glm::vec3 bbox[8]{
            glm::vec3(scene_min.x, scene_min.y, scene_min.z),
            glm::vec3(scene_min.x, scene_min.y, scene_max.z),
            glm::vec3(scene_min.x, scene_max.y, scene_min.z),
            glm::vec3(scene_min.x, scene_max.y, scene_max.z),
            glm::vec3(scene_max.x, scene_min.y, scene_min.z),
            glm::vec3(scene_max.x, scene_min.y, scene_max.z),
            glm::vec3(scene_max.x, scene_max.y, scene_min.z),
            glm::vec3(scene_max.x, scene_max.y, scene_max.z),
        };

        const glm::vec3 target = (scene_min + scene_max) * 0.5f;
        for (size_t i_light = 0; i_light < m_num_lights; i_light++)
        {
            glm::vec3 current_dir = m_dirs[i_light];

            // view matrix
            float dist = glm::length(scene_max - scene_min);
            glm::vec3 center = target + current_dir * dist;

            int right_axis = std::fabs(current_dir[1]) < std::fabs(current_dir[0]) ? 1 : 0;
            right_axis = std::fabs(current_dir[2]) < std::fabs(current_dir[right_axis]) ? 2 : right_axis;
            glm::vec3 right(0.0f);
            right[right_axis] = 1.0f;

            glm::vec3 up = glm::normalize(glm::cross(right, current_dir));
            glm::mat4 current_view_mat = glm::lookAt(center, target, up);
            m_view_mat.push_back(current_view_mat);

            // projection matrix
            float xy_range = 0.0f, zmax = 0.0f, zmin = 0.0f;    // modified by shen
            for (int j_bv = 0; j_bv < 8; j_bv++)
            {
                glm::vec3 view_vert = current_view_mat * glm::vec4(bbox[j_bv], 1.0f);
                xy_range = std::max(xy_range, std::fabs(view_vert.x));
                xy_range = std::max(xy_range, std::fabs(view_vert.y));

                view_vert.z = -view_vert.z;
                zmin = (j_bv == 0) ? view_vert.z : std::min(zmin, view_vert.z);
                zmax = (j_bv == 0) ? view_vert.z : std::max(zmax, view_vert.z);
            }

            glm::mat4 current_proj_mat = glm::ortho(-xy_range, xy_range, -xy_range, xy_range, zmin - 10.f, zmax + 10.0f);
            m_proj_mat.push_back(current_proj_mat);

            glm::mat4 current_view_proj_mat = current_proj_mat * current_view_mat;
            m_view_proj_mat.push_back(current_view_proj_mat);
        }
    }

    void update(Shader* shader) override
    {

    }

private:
    std::vector<glm::vec3> m_ori_colors;
    std::vector<glm::vec3> m_ori_dirs;

public:
    int m_num_lights = 0;
    float m_strength = 0.f;
    std::vector<glm::vec3> m_colors;
    std::vector<glm::vec3> m_dirs;

    std::vector<glm::mat4> m_view_mat;
    std::vector<glm::mat4> m_proj_mat;
    std::vector<glm::mat4> m_view_proj_mat;
    // it is better for setting as private parameters and using get function
};