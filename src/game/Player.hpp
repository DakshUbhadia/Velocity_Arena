#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Input;

class Player {
public:
    Player(const glm::vec3& position, float movementSpeed);

    void update(const Input& input, float deltaTime);

    glm::mat4 modelMatrix() const;

    const glm::vec3& position() const;

private:
    glm::vec3 position_;
    float movementSpeed_;
};
