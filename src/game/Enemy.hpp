#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "physics/AABB.hpp"

enum class EnemyState {
    Idle,
    Chase,
    Attack
};

class Enemy {
public:
    // Rendered size: 1x1x1 cube. Half-extents = 0.5 on each axis.
    static constexpr float kHalfSize = 0.5f;

    Enemy(const glm::vec3& position, float movementSpeed);

    // Updates enemy FSM and movement. Returns true if attack is triggered.
    bool update(const glm::vec3& playerPosition, float deltaTime);

    void kill();

    bool alive() const;

    EnemyState state() const;

    const glm::vec3& position() const;

    glm::mat4 modelMatrix() const;

    // Returns an AABB in world space matching the enemy's rendered cube.
    AABB bounds() const;

private:
    glm::vec3 position_;
    float movementSpeed_;
    EnemyState state_{EnemyState::Idle};
    bool alive_{true};

    float detectionRadius_{6.0f};
    float attackRange_{1.0f};  // ~1.0 world unit for unit-cube enemies -- gameplay tuning
    float attackCooldown_{0.0f};
    float attackInterval_{0.75f};
};
