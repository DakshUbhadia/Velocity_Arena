#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/Shader.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Primitives.hpp"
#include "graphics/Camera.hpp"

#include "core/GameClock.hpp"
#include "core/Input.hpp"

#include "game/Game.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Velocity Arena", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << "\n";
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";

    // Ensure OpenGL resources are destroyed before the context is terminated
    try {
        {
            Renderer renderer;
        
        Shader shader("shaders/basic.vert", "shaders/basic.frag");
        Mesh cubeMesh(Primitives::cubeVertices);
        Mesh floorMesh(Primitives::planeVertices);

        GameClock clock;
        Input input(window);
        
        Game game;
        
        // Initial Camera Setup
        glm::vec3 cameraOffset(0.0f, 6.0f, 8.0f);
        Camera camera(game.player().position() + cameraOffset, game.player().position(), glm::vec3(0.0f, 1.0f, 0.0f));

        double lastTitleUpdateTime = 0.0;
        int frameCount = 0;

        while (!glfwWindowShouldClose(window)) {
            // 1. Poll Events
            glfwPollEvents();

            // 2. Tick Game Clock
            clock.tick();
            float dt = clock.deltaTime();

            // Handle Escape manually
            if (input.isKeyDown(GLFW_KEY_ESCAPE)) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            // 3. Update Game Logic
            game.update(input, dt);

            // Update Camera to follow player
            camera.setPosition(game.player().position() + cameraOffset);
            camera.setTarget(game.player().position());

            // Update Title (FPS and ms per frame)
            frameCount++;
            if (clock.elapsedTime() - lastTitleUpdateTime >= 1.0) {
                double fps = frameCount / (clock.elapsedTime() - lastTitleUpdateTime);
                
                std::ostringstream title;
                if (game.state() == GameState::Running) {
                    title << "Velocity Arena | HP: " << game.player().health() << "/" << game.player().maxHealth()
                          << " | Score: " << game.score()
                          << " | Enemies: " << game.enemies().size()
                          << " | FPS: " << static_cast<int>(fps);
                } else {
                    title << "Velocity Arena | GAME OVER"
                          << " | Score: " << game.score()
                          << " | Press R to Restart";
                }
                glfwSetWindowTitle(window, title.str().c_str());
                
                lastTitleUpdateTime = clock.elapsedTime();
                frameCount = 0;
            }

            // 4. Calculate Matrices
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            if (width <= 0 || height <= 0) {
                glfwWaitEvents();
                continue;
            }

            float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
            glm::mat4 view = camera.viewMatrix();

            // 5. Render
            renderer.clear();

            // Render Floor (Neutral Gray)
            glm::mat4 floorModel = glm::mat4(1.0f);
            floorModel = glm::scale(floorModel, glm::vec3(10.0f, 1.0f, 10.0f));
            renderer.draw(floorMesh, shader, floorModel, view, projection, glm::vec3(0.4f, 0.4f, 0.4f));

            // Render Player (Bright Green/Blue as suggested)
            if (game.player().isAlive()) {
                renderer.draw(cubeMesh, shader, game.player().modelMatrix(), view, projection, glm::vec3(0.2f, 1.0f, 0.2f));
            }

            // Render Enemies
            for (const Enemy& enemy : game.enemies()) {
                if (!enemy.alive()) continue;
                glm::vec3 color(1.0f, 0.0f, 0.0f); // Default to red
                if (enemy.state() == EnemyState::Idle) color = glm::vec3(0.8f, 0.8f, 0.2f); // yellowish
                else if (enemy.state() == EnemyState::Chase) color = glm::vec3(1.0f, 0.5f, 0.0f); // orange
                else if (enemy.state() == EnemyState::Attack) color = glm::vec3(1.0f, 0.0f, 0.0f); // strong red
                
                renderer.draw(cubeMesh, shader, enemy.modelMatrix(), view, projection, color);
            }

            // Render Projectiles
            for (const Projectile& proj : game.projectiles()) {
                if (!proj.active()) continue;
                renderer.draw(cubeMesh, shader, proj.modelMatrix(), view, projection, glm::vec3(0.0f, 1.0f, 1.0f)); // Cyan
            }

            // 6. Swap Buffers
            glfwSwapBuffers(window);
        }
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
