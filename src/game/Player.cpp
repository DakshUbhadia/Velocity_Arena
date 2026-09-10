#include "game/Player.hpp"
#include "core/Input.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Player::Player(const glm::vec3& position, float movementSpeed)
    : position_(position), movementSpeed_(movementSpeed) {
}

void Player::update(const Input& input, float deltaTime) {
    glm::vec3 movement(0.0f);

    if (input.isKeyDown(GLFW_KEY_W)) {
        movement.z -= 1.0f;
    }
    if (input.isKeyDown(GLFW_KEY_S)) {
        movement.z += 1.0f;
    }
    if (input.isKeyDown(GLFW_KEY_A)) {
        movement.x -= 1.0f;
    }
    if (input.isKeyDown(GLFW_KEY_D)) {
        movement.x += 1.0f;
    }

    if (glm::length(movement) > 0.0f) {
        movement = glm::normalize(movement);
    }

    position_ += movement * movementSpeed_ * deltaTime;

    // Temporary arena bounds
    position_.x = std::clamp(position_.x, -4.5f, 4.5f);
    position_.z = std::clamp(position_.z, -4.5f, 4.5f);
}

glm::mat4 Player::modelMatrix() const {
    glm::mat4 model(1.0f);
    model = glm::translate(model, position_);
    // Base is 1 unit cube, place base roughly on the floor. 
    // Position represents the center, so no additional scaling/offset is needed
    // if the default cube is from -0.5 to 0.5 and floor is at y=0.
    return model;
}

const glm::vec3& Player::position() const {
    return position_;
}
