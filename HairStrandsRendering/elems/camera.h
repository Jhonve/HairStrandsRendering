#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "elems/input.h"
#include "elems/element.h"
#include "render/shader.h"

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
        glm::mat4 model{ 1.0f };
        shader->set_mat4(model, "model_mat");
        shader->set_mat4(m_view_matrix, "view_mat");
        shader->set_mat4(get_projection(), "proj_mat");
        shader->set_vec3(m_position, "cam_pos");
    }
    
    void set_aspect(float aspect)
    {
        m_projection = glm::perspective(m_fov, aspect, m_near, m_far);
    }
    
    void set_distance(float offset)
    {
        m_distance += offset;
        update_view_matrix();
    }
    
    const glm::mat4& get_projection() const
    {
        return m_projection;
    }
    
    glm::mat4 get_view_projection() const
    {
        return m_projection * get_view_matrix();
    }
    
    glm::vec3 get_up() const
    {
        return glm::rotate(get_direction(), m_up);
    }
    
    glm::vec3 get_right() const
    {
        return glm::rotate(get_direction(), m_right);
    }
    
    glm::vec3 get_forward() const
    {
        return glm::rotate(get_direction(), m_forward);
    }
    
    glm::quat get_direction() const
    {
        return glm::quat(glm::vec3(-m_pitch, -m_yaw, 0.0f));
    }
    
    glm::mat4 get_view_matrix() const
    {
        return m_view_matrix;
    }
    
    void on_mouse_wheel(double delta)
    {
        set_distance(delta * 0.5f);
        update_view_matrix();
    }
    
    void reset()
    {
        m_focus = { 0.0f, 0.0f, 0.0f };
        //mDistance = 5.0f;
        update_view_matrix();
    }
    
    void on_mouse_move(double x, double y, InputButton button)
    {
        glm::vec2 pos2d{ x, y };
    
        if (button == InputButton::Right)
        {
            glm::vec2 delta = (pos2d - m_current_pos2d) * 0.004f;
    
            float sign = get_up().y < 0 ? -1.0f : 1.0f;
            m_yaw += sign * delta.x * m_rot_speed;
            m_pitch += delta.y * m_rot_speed;
    
            update_view_matrix();
        }	
        else if (button == InputButton::Middle)
        {
            // TODO: Adjust pan speed for distance
            glm::vec2 delta = (pos2d - m_current_pos2d) * 0.003f;
    
            m_focus += -get_right() * delta.x * m_distance;
            m_focus += get_up() * delta.y * m_distance;
    
            update_view_matrix();
    }
    
    m_current_pos2d = pos2d;
    }
    
    void update_view_matrix()
    {
        m_position =  m_focus - get_forward() * m_distance;
    
        glm::quat orientation = get_direction();
        m_view_matrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);
        m_view_matrix = glm::inverse(m_view_matrix);
    }

private:
    glm::mat4 m_view_matrix;
    glm::mat4 m_projection = glm::mat4{ 1.0f };
    glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
    
    glm::vec3 m_focus = { 0.0f, 0.0f, 0.0f };
    
    float m_distance = 5.0f;
    float m_aspect;
    float m_fov;
    float m_near;
    float m_far;

    float m_pitch = 0.0f;
    float m_yaw = 0.0f;

    glm::vec2 m_current_pos2d = { 0.0f, 0.0f };

    const glm::vec3 m_right = { 1.0f, 0.0f, 0.0f };
    const glm::vec3 m_up = { 0.0f, 1.0f, 0.0f };
    const glm::vec3 m_forward = { 0.0f, 0.0f, -1.0f };

    const float m_rot_speed = 2.0f;

};