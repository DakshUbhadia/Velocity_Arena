#include "graphics/Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    : position_(position), target_(target), up_(up) {
}

void Camera::setPosition(const glm::vec3& position) {
    position_ = position;
}

void Camera::setTarget(const glm::vec3& target) {
    target_ = target;
}

const glm::vec3& Camera::position() const {
    return position_;
}

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(position_, target_, up_);
}
