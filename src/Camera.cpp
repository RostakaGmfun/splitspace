#include <splitspace/Camera.hpp>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace splitspace {

Camera::Camera(float w, float h, float fov, float near, float far):
               m_width(w), m_height(h), m_fov(fov), m_near(near), m_far(far)
{}

FPSCamera::FPSCamera(float w, float h, float fov, float near, float far):
                     Camera(w, h, fov, near, far), m_speed(0), m_moveVertically(false)
{}

bool FPSCamera::init() {
    m_projMat = glm::perspective(m_fov, m_width/m_height, m_near, m_far);
    return true;
}

void FPSCamera::update(float dt) {
    auto prevPos = m_position;
    m_position.x+=m_speed*dt*glm::cos(m_rotation.z*glm::pi<float>()/180.f);
    m_position.y+=m_speed*dt*glm::sin(m_rotation.z*glm::pi<float>()/180.f);

    if(m_moveVertically) {
        m_position.z+=m_speed*dt*glm::cos(m_rotation.x*glm::pi<float>()/180.f);
    }

    m_viewProj = m_projMat*glm::lookAt(prevPos, m_position, glm::vec3(0,1,0));
}

void FPSCamera::destroy() {

}

} // namespace splitspace

