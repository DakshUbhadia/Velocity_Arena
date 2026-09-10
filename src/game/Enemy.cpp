#include "Enemy.hpp"

Enemy::Enemy(const glm::vec3& position, float movementSpeed)
    : position_(position), movementSpeed_(movementSpeed) {
}

bool Enemy::update(const glm::vec3& playerPosition, float deltaTime) {
    if (!alive_) return false;

    float dx = playerPosition.x - position_.x;
    float dz = playerPosition.z - position_.z;
    float distanceSquared = dx * dx + dz * dz;

    // State transitions
    if (distanceSquared > detectionRadius_ * detectionRadius_) {
        state_ = EnemyState::Idle;
    } else if (distanceSquared > attackRange_ * attackRange_) {
        state_ = EnemyState::Chase;
    } else {
        state_ = EnemyState::Attack;
    }

    bool attackTriggered = false;

    // Behavior
    switch (state_) {
        case EnemyState::Idle:
            // Do nothing
            break;
        case EnemyState::Chase: {
            glm::vec3 direction = playerPosition - position_;
            direction.y = 0.0f; // Keep on XZ plane
            if (glm::length(direction) > 0.0f) {
                direction = glm::normalize(direction);
                position_ += direction * movementSpeed_ * deltaTime;
            }
            break;
        }
        case EnemyState::Attack: {
            attackCooldown_ -= deltaTime;
            if (attackCooldown_ <= 0.0f) {
                attackTriggered = true;
                attackCooldown_ = attackInterval_;
            }
            break;
        }
    }

    return attackTriggered;
}

void Enemy::kill() {
    alive_ = false;
}

bool Enemy::alive() const {
    return alive_;
}

EnemyState Enemy::state() const {
    return state_;
}

const glm::vec3& Enemy::position() const {
    return position_;
}

glm::mat4 Enemy::modelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position_);
    // No scaling needed for Enemy as per plan, just using base cube size
    return model;
}
