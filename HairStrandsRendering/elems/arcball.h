#pragma once

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

class ArcBall
{
public:
    ArcBall() : m_width(0.0f), m_height(0.0f), m_focus(glm::vec3(0.f)), m_inv_view_mat(glm::mat4(0.f))
    {
    }
    ~ArcBall()
    {
    }

    void set_win_size(float width, float height);
    void set_focus(const glm::vec3 focus) { m_focus = focus; }
    void set_inv_view_mat(const glm::mat4 inv_view_mat) { m_inv_view_mat = inv_view_mat; }
    void set_rot_mat(const glm::mat3 rot_mat) { m_rot_mat = rot_mat; }

    void click(float x, float y);
    void release();
    void rotate(float x, float y);

    void reset();

    const glm::vec3 get_rot_focus() const { return m_focus; }
    const glm::mat3 get_rot_mat() const { return m_rot_mat; }
    const glm::mat4 get_trans_mat() const;
	
private:
    const glm::vec3 map_to_sphere_coord(const float x, const float y);
    void update_rot_mat();

private:
    float m_speed = 0.03f;
    float m_width;
    float m_height;
    glm::vec3 m_focus{0.f};
    glm::mat4 m_inv_view_mat;

    float m_last_x;
    float m_last_y;

    float m_curr_angle;
    glm::vec3 m_curr_axis;

    glm::mat4 m_trans_mat;
    glm::mat3 m_rot_mat{1.0f};
};