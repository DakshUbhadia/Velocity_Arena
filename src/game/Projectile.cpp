#include "Projectile.hpp"

Projectile::Projectile(const glm::vec3& position,
                       const glm::vec3& direction,
                       float speed,
                       float lifetime)
    : position_(position), speed_(speed), lifetimeRemaining_(lifetime) {
    if (glm::length(direction) > 0.0f) {
        direction_ = glm::normalize(direction);
    } else {
        direction_ = glm::vec3(0.0f, 0.0f, -1.0f); // Fallback
    }
}

void Projectile::update(float deltaTime) {
    position_ += direction_ * speed_ * deltaTime;
    lifetimeRemaining_ -= deltaTime;
    if (lifetimeRemaining_ <= 0.0f) {
        active_ = false;
    }
}

void Projectile::deactivate() {
    active_ = false;
}

bool Projectile::active() const {
    return active_;
}

const glm::vec3& Projectile::position() const {
    return position_;
}

glm::mat4 Projectile::modelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position_);
    // Scale cube to 0.2 units (0.15-0.25 was suggested)
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
    return model;
}
