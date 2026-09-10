#include "Enemy.hpp"

Enemy::Enemy(const glm::vec3& position, float movementSpeed)
    : position_(position), movementSpeed_(movementSpeed) {
}

bool Enemy::update(const glm::vec3& playerPosition, float deltaTime) {
    if (!alive_) return false;

    float dx = playerPosition.x - position_.x;
    float dz = playerPosition.z - position_.z;
    float distanceSquared = dx * dx + dz * dz;

    // State transitions based on squared distance thresholds.
    // Note: attackRange_ is gameplay/AI distance (not AABB intersection).
    if (distanceSquared > detectionRadius_ * detectionRadius_) {
        state_ = EnemyState::Idle;
    } else if (distanceSquared > attackRange_ * attackRange_) {
        state_ = EnemyState::Chase;
    } else {
        state_ = EnemyState::Attack;
    }

    bool attackTriggered = false;

    switch (state_) {
        case EnemyState::Idle:
            break;
        case EnemyState::Chase: {
            glm::vec3 direction = playerPosition - position_;
            direction.y = 0.0f;
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

void Enemy::kill() { alive_ = false; }
bool Enemy::alive() const { return alive_; }
EnemyState Enemy::state() const { return state_; }
const glm::vec3& Enemy::position() const { return position_; }

glm::mat4 Enemy::modelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position_);
    return model;
}

AABB Enemy::bounds() const {
    // The enemy is rendered as a 1x1x1 cube centered at position_.
    // Half-extents = (0.5, 0.5, 0.5). kHalfSize keeps bounds() and
    // modelMatrix() derived from the same conceptual size.
    return AABB::fromCenterHalfExtents(
        position_,
        glm::vec3(kHalfSize, kHalfSize, kHalfSize)
    );
}
