#pragma once

struct GLFWwindow;

class Input {
public:
    explicit Input(GLFWwindow* window);

    bool isKeyDown(int key) const;

private:
    // Non-owning pointer to the window. Application handles lifetime.
    GLFWwindow* window_;
};
