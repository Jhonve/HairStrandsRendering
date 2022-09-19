#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "elems/input.h"
#include "elems/element.h"
#include "elems/arcball.h"

class Camera : public Element
{
public:
    Camera(const glm::vec3& position, float fov, float aspect, float near, float far)
    {
        m_position = position;
        m_aspect = aspect;
        m_near = near;
        m_far = far;
        m_fov = fov;
    
        set_aspect(m_aspect);
    
        update_view_matrix();
    }
    
    void update(Shader* shader) override
    {
        shader->set_mat4(m_model_mat, "model_mat");
        shader->set_mat4(m_view_mat, "view_mat");
        shader->set_mat4(m_proj_mat, "proj_mat");
        shader->set_vec3(m_position, "cam_pos");
    }

    void update_mat(Shader* shader)
    {
        shader->set_mat4(m_model_mat, "model_mat");
        shader->set_mat4(m_view_mat, "view_mat");
        shader->set_mat4(m_proj_mat, "proj_mat");
    }

    void setup(float width, float height)
    {
        m_width = width;
        m_height = height;
        set_aspect(m_width / m_height);
        m_arcball.set_win_size(m_width, m_height);
        m_arcball.set_inv_view_mat(glm::inverse(m_view_mat));
    }
    
    void set_aspect(float aspect)
    {
        m_proj_mat = glm::perspective(m_fov, aspect, m_near, m_far);
    }
    
    void set_distance(float offset)
    {
        m_distance += offset;
        update_view_matrix();
    }
    
    glm::mat4 get_view_projection() const{ return m_proj_mat * get_view_mat(); }
    
    glm::vec3 get_up() const { return m_up; }
    
    glm::vec3 get_right() const { return m_right; }

    glm::vec3 get_forward() const { return m_forward; }
    
    glm::mat4 get_model_mat() const { return m_model_mat; }
    
    glm::mat4 get_view_mat() const { return m_view_mat; }
    
    void on_mouse_wheel(double delta)
    {
        set_distance(delta * 0.5f);
        update_view_matrix();
    }
    
    void reset()
    {
        m_arcball.reset();
        m_model_mat = m_arcball.get_trans_mat();
        m_focus = { 0.0f, 0.0f, 0.0f };
        m_distance = 5.0f;
        update_view_matrix();
    }
    
    void on_mouse_move(double x, double y, InputButton button)
    {
        glm::vec2 pos2d{ x, y };
    
        if (button == InputButton::Right)
        {
            m_arcball.click(m_current_pos2d.x, m_current_pos2d.y);
            m_arcball.rotate(x, y);
            m_model_mat = m_arcball.get_trans_mat();
            m_arcball.release();
        }	
        else if (button == InputButton::Middle)
        {
            glm::vec2 delta = (pos2d - m_current_pos2d) * 0.003f;
            m_focus += -get_right() * delta.x * m_distance * 0.3f;
            m_focus += get_up() * delta.y * m_distance * 0.3f;
            update_view_matrix();
        }
    
        m_current_pos2d = pos2d;
    }
    
    void update_view_matrix()
    {
        m_position =  m_focus - get_forward() * m_distance;
        m_view_mat = glm::translate(glm::mat4(1.0f), m_position);
        m_view_mat = glm::inverse(m_view_mat);
    }

private:
    float m_width = 0;
    float m_height = 0;

    glm::mat4 m_model_mat{ 1.0f }; 
    glm::mat4 m_view_mat;
    glm::mat4 m_proj_mat{ 1.0f };

    glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_focus = { 0.0f, 0.0f, 0.0f };
    
    float m_distance = 5.0f;
    float m_aspect;
    float m_fov;
    float m_near;
    float m_far;

    glm::vec2 m_current_pos2d = { 0.0f, 0.0f };

    const glm::vec3 m_right = { 1.0f, 0.0f, 0.0f };
    const glm::vec3 m_up = { 0.0f, 1.0f, 0.0f };
    const glm::vec3 m_forward = { 0.0f, 0.0f, -1.0f };

    ArcBall m_arcball;
};