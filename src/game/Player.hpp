#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Input;

class Player {
public:
    Player(const glm::vec3& position,
           float movementSpeed,
           int maxHealth = 100);

    void update(const Input& input, float deltaTime);

    void takeDamage(int amount);
    void reset(const glm::vec3& position);

    glm::mat4 modelMatrix() const;

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
