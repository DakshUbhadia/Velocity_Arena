#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Projectile {
public:
    Projectile(const glm::vec3& position,
               const glm::vec3& direction,
               float speed,
               float lifetime);

    void update(float deltaTime);

    void deactivate();

    bool active() const;

    const glm::vec3& position() const;

    glm::mat4 modelMatrix() const;

private:
    glm::vec3 position_;
    glm::vec3 direction_;

    float speed_;
    float lifetimeRemaining_;

    bool active_{true};
};
