#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/matrix.hpp>

namespace splitspace {

class Camera {
public:
    Camera(float w, float h, float fov, float near, float far);
    virtual ~Camera() {}

    virtual bool init() = 0;
    virtual void update(float dt) = 0;
    virtual void destroy() = 0;

    void setPosition(const glm::vec3 &pos) { m_position = pos; }
    void setRotation(const glm::vec3 &rot) { m_rotation = rot; }

    const glm::mat4 &getVP() const { return m_viewProj; }
    const glm::vec3 &getPosition() const { return m_position; }
    const glm::vec3 &getRotation() const { return m_rotation; }

protected:
    glm::mat4 m_viewProj;
    glm::mat4 m_projMat;
    glm::vec3 m_position;
    glm::vec3 m_rotation;

    float m_width;
    float m_height;
    float m_fov;

    float m_near;
    float m_far;
};

class FPSCamera: public Camera {
public:
    FPSCamera(float w, float h, float fov, float near, float far);

    ~FPSCamera() { destroy(); }

    bool init();
    void update(float dt);
    void destroy();

    void setSpeed(float speed) { m_speed = speed; }
    void setMoveVertically(bool p) { m_moveVertically = p; }

private:
    float m_speed;
    bool m_moveVertically;
};

class LookatCamera: public Camera {
public:
    LookatCamera(float w, float h, float fov, float near, float far);

    ~LookatCamera() { destroy(); }

    bool init();
    void update(float dt);
    void destroy();

    void setLookPosition(const glm::vec3 &lookPos) { m_lookPos = lookPos; }

private:
    glm::vec3 m_lookPos;
};

} //namepsace splitspace

#endif //CAMERA_HPP
