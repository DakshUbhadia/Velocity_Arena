#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "physics/AABB.hpp"

class Projectile {
public:
    // kSize is the full side length of the rendered projectile cube.
    // modelMatrix() scales by kSize; bounds() derives half-extents from kSize/2.
    // Using a single constant prevents visual/collision sizes from silently diverging.
    static constexpr float kSize     = 0.2f;
    static constexpr float kHalfSize = kSize * 0.5f;  // 0.1f

    Projectile(const glm::vec3& position,
               const glm::vec3& direction,
               float speed,
               float lifetime);

    void update(float deltaTime);

    void deactivate();

    bool active() const;

    const glm::vec3& position() const;

    glm::mat4 modelMatrix() const;

    // Returns an AABB in world space matching the projectile's rendered cube.
    AABB bounds() const;

private:
    glm::vec3 position_;
    glm::vec3 direction_;

    float speed_;
    float lifetimeRemaining_;

    bool active_{true};
};
