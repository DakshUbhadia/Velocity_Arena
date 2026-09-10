#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "physics/AABB.hpp"

class Input;

class Player {
public:
    // Rendered size: 1x1x1 cube. Half-extents = 0.5 on each axis.
    static constexpr float kHalfSize = 0.5f;

    Player(const glm::vec3& position,
           float movementSpeed,
           int maxHealth = 100);

    void update(const Input& input, float deltaTime);

    void takeDamage(int amount);
    void reset(const glm::vec3& position);

    glm::mat4 modelMatrix() const;

    // Returns an AABB in world space matching the player's rendered cube.
    AABB bounds() const;

    const glm::vec3& position() const;
    const glm::vec3& facingDirection() const;

    int health() const;
    int maxHealth() const;
    bool isAlive() const;

private:
    glm::vec3 position_;
    glm::vec3 facingDirection_{0.0f, 0.0f, -1.0f};

    float movementSpeed_;

    int health_;
    int maxHealth_;
};
