#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Camera {
public:
    Camera(const glm::vec3& position,
           const glm::vec3& target,
           const glm::vec3& up);

    void setPosition(const glm::vec3& position);
    void setTarget(const glm::vec3& target);

    const glm::vec3& position() const;

    glm::mat4 viewMatrix() const;

private:
    glm::vec3 position_;
    glm::vec3 target_;
    glm::vec3 up_;
};
