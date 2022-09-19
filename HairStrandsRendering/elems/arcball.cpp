#include "elems/arcball.h"
#include <algorithm>

void ArcBall::set_win_size(float width, float height)
{
    m_width = width;
    m_height = height;
}

void ArcBall::click(float x, float y)
{
    m_last_x = x;
    m_last_y = y;
}

void ArcBall::release()
{
    m_curr_angle = 0.0f;
}

void ArcBall::rotate(float x, float y)
{
    x = std::max(std::min(x, m_width - 1.f), 1.f);
    y = std::max(std::min(y, m_height - 1.f), 1.f);

    float x_offset = x - m_last_x;
    float y_offset = y - m_last_y;

    if ((std::abs(x_offset)) < std::numeric_limits<float>::epsilon() && 
        (std::abs(y_offset)) < std::numeric_limits<float>::epsilon())
    {
        m_curr_angle = 0.0f;
    }
    else
    {
        glm::vec3 pre_sphere_pos = this->map_to_sphere_coord(m_last_x, m_last_y);
        glm::vec3 curr_sphere_pos = this->map_to_sphere_coord(x, y);

        m_curr_angle = std::acos(std::min(1.0f, glm::dot(pre_sphere_pos, curr_sphere_pos)));
        m_curr_axis = glm::normalize(glm::cross(pre_sphere_pos, curr_sphere_pos));

        m_last_x = x;
        m_last_y = y;
        this->update_rot_mat();
    }
}

const glm::vec3 ArcBall::map_to_sphere_coord(const float x, const float y)
{
    glm::vec3 coord(0.0f, 0.0f, 0.0f);

    coord.x = (2 * x - m_width) / m_width;
    coord.y = -(2 * y - m_height) / m_height;

    coord.x = glm::clamp(coord.x, -1.0f, 1.0f);
    coord.y = glm::clamp(coord.y, -1.0f, 1.0f);

    float length = coord.x * coord.x + coord.y * coord.y;

    if (length <= 1.0f)
        coord.z = std::sqrt(1.0f - length);
    else
        coord = glm::normalize(coord);

    return coord;
}

void ArcBall::update_rot_mat()
{
    m_curr_axis = glm::inverse(glm::mat3(m_inv_view_mat)) * m_curr_axis;
    glm::mat3 curr_rot_mat = glm::rotate(glm::degrees(m_curr_angle) * m_speed, m_curr_axis);
    m_rot_mat = curr_rot_mat * m_rot_mat;
}

const glm::mat4 ArcBall::get_trans_mat() const
{
    return glm::translate(glm::mat4(1.0f), m_focus) * glm::mat4(m_rot_mat) *  glm::translate(glm::mat4(1.0f), -m_focus);
}

void ArcBall::reset()
{
    m_rot_mat = glm::mat3(1.0f);
}