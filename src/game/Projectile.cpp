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

void Projectile::deactivate() { active_ = false; }
bool Projectile::active() const { return active_; }
const glm::vec3& Projectile::position() const { return position_; }

glm::mat4 Projectile::modelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position_);
    // Scale the unit cube to kSize on all axes.
    // This constant is also used by bounds() to keep visuals and collision in sync.
    model = glm::scale(model, glm::vec3(kSize, kSize, kSize));
    return model;
}

AABB Projectile::bounds() const {
    // The projectile is rendered as a kSize x kSize x kSize cube centered at position_.
    // Half-extents = (kHalfSize, kHalfSize, kHalfSize) = (0.1, 0.1, 0.1).
    // kHalfSize is derived from kSize so bounds() stays consistent with modelMatrix().
    return AABB::fromCenterHalfExtents(
        position_,
        glm::vec3(kHalfSize, kHalfSize, kHalfSize)
    );
}
