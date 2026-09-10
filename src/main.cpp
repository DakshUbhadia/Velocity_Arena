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

#include "game/Player.hpp"

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
        return -1;
    }

    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << "\n";
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";

    // Ensure OpenGL resources are destroyed before the context is terminated
    {
        Renderer renderer;
        
        Shader shader("shaders/basic.vert", "shaders/basic.frag");
        Mesh cubeMesh(Primitives::cubeVertices);
        Mesh floorMesh(Primitives::planeVertices);

        GameClock clock;
        Input input(window);
        
        // Initial Player Position at origin, speed of 5.0f
        Player player(glm::vec3(0.0f, 0.5f, 0.0f), 5.0f);
        
        // Initial Camera Setup
        glm::vec3 cameraOffset(0.0f, 6.0f, 8.0f);
        Camera camera(player.position() + cameraOffset, player.position(), glm::vec3(0.0f, 1.0f, 0.0f));

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
            player.update(input, dt);

            // Update Camera to follow player
            camera.setPosition(player.position() + cameraOffset);
            camera.setTarget(player.position());

            // Update Title (FPS and ms per frame)
            frameCount++;
            if (clock.elapsedTime() - lastTitleUpdateTime >= 1.0) {
                double fps = frameCount / (clock.elapsedTime() - lastTitleUpdateTime);
                double ms = 1000.0 / fps;
                
                std::ostringstream title;
                title << "Velocity Arena | FPS: " << static_cast<int>(fps) 
                      << " | Frame: " << static_cast<int>(ms) << " ms";
                glfwSetWindowTitle(window, title.str().c_str());
                
                lastTitleUpdateTime = clock.elapsedTime();
                frameCount = 0;
            }

            // 4. Calculate Matrices
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            if (height == 0) height = 1; // Prevent division by zero

            float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
            glm::mat4 view = camera.viewMatrix();

            // 5. Render
            renderer.clear();

            // Render Floor (Neutral Gray)
            glm::mat4 floorModel = glm::mat4(1.0f);
            floorModel = glm::scale(floorModel, glm::vec3(10.0f, 1.0f, 10.0f));
            renderer.draw(floorMesh, shader, floorModel, view, projection, glm::vec3(0.4f, 0.4f, 0.4f));

            // Render Static Objects (Remaining cubes from Milestone 1, colored differently)
            glm::mat4 cube2Model = glm::mat4(1.0f);
            cube2Model = glm::translate(cube2Model, glm::vec3(-2.0f, 0.5f, -2.0f));
            cube2Model = glm::rotate(cube2Model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            renderer.draw(cubeMesh, shader, cube2Model, view, projection, glm::vec3(0.2f, 0.8f, 0.2f));

            glm::mat4 cube3Model = glm::mat4(1.0f);
            cube3Model = glm::translate(cube3Model, glm::vec3(2.0f, 0.5f, -1.0f));
            cube3Model = glm::rotate(cube3Model, glm::radians(25.0f), glm::vec3(1.0f, 1.0f, 0.0f));
            cube3Model = glm::scale(cube3Model, glm::vec3(1.5f, 1.5f, 1.5f));
            renderer.draw(cubeMesh, shader, cube3Model, view, projection, glm::vec3(0.2f, 0.2f, 0.8f));

            // Render Player (Bright Red)
            glm::mat4 playerModel = player.modelMatrix();
            renderer.draw(cubeMesh, shader, playerModel, view, projection, glm::vec3(1.0f, 0.2f, 0.2f));

            // 6. Swap Buffers
            glfwSwapBuffers(window);
        }
    }

    glfwTerminate();
    return 0;
}
