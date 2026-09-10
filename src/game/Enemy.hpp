#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class EnemyState {
    Idle,
    Chase,
    Attack
};

class Enemy {
public:
    Enemy(const glm::vec3& position, float movementSpeed);

    bool update(const glm::vec3& playerPosition, float deltaTime);

    void kill();

    bool alive() const;

    EnemyState state() const;

    const glm::vec3& position() const;

    glm::mat4 modelMatrix() const;

private:
    glm::vec3 position_;
    float movementSpeed_;
    EnemyState state_{EnemyState::Idle};
    bool alive_{true};

    float detectionRadius_{6.0f};
    float attackRange_{0.8f};
    float attackCooldown_{0.0f};
    float attackInterval_{0.75f};
};
