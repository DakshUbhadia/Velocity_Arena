#include "game/Player.hpp"
#include "core/Input.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Player::Player(const glm::vec3& position, float movementSpeed, int maxHealth)
    : position_(position), movementSpeed_(movementSpeed), health_(maxHealth), maxHealth_(maxHealth) {
}

void Player::update(const Input& input, float deltaTime) {
    glm::vec3 movement(0.0f);

    if (input.isKeyDown(GLFW_KEY_W)) { movement.z -= 1.0f; }
    if (input.isKeyDown(GLFW_KEY_S)) { movement.z += 1.0f; }
    if (input.isKeyDown(GLFW_KEY_A)) { movement.x -= 1.0f; }
    if (input.isKeyDown(GLFW_KEY_D)) { movement.x += 1.0f; }

    if (glm::length(movement) > 0.0f) {
        movement = glm::normalize(movement);
        facingDirection_ = movement;
    }

    position_ += movement * movementSpeed_ * deltaTime;

    // Arena boundary clamp
    position_.x = std::clamp(position_.x, -4.5f, 4.5f);
    position_.z = std::clamp(position_.z, -4.5f, 4.5f);
}

glm::mat4 Player::modelMatrix() const {
    glm::mat4 model(1.0f);
    model = glm::translate(model, position_);
    return model;
}

AABB Player::bounds() const {
    // The player is rendered as a 1x1x1 cube centered at position_.
    // Half-extents = (0.5, 0.5, 0.5) on all axes.
    // kHalfSize is also used here so bounds() and modelMatrix() stay in sync.
    return AABB::fromCenterHalfExtents(
        position_,
        glm::vec3(kHalfSize, kHalfSize, kHalfSize)
    );
}

void Player::takeDamage(int amount) {
    if (amount > 0) {
        health_ = std::max(0, health_ - amount);
    }
}

void Player::reset(const glm::vec3& position) {
    position_ = position;
    health_ = maxHealth_;
    facingDirection_ = glm::vec3(0.0f, 0.0f, -1.0f);
}

const glm::vec3& Player::facingDirection() const { return facingDirection_; }
int Player::health() const { return health_; }
int Player::maxHealth() const { return maxHealth_; }
bool Player::isAlive() const { return health_ > 0; }
const glm::vec3& Player::position() const { return position_; }
