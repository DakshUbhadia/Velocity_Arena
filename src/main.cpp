#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/Shader.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Primitives.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

        while (!glfwWindowShouldClose(window)) {
            processInput(window);

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            if (height == 0) height = 1; // Prevent division by zero

            float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

            glm::mat4 view = glm::lookAt(
                glm::vec3(0.0f, 4.0f, 8.0f), // Camera position
                glm::vec3(0.0f, 0.0f, 0.0f), // Target position
                glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
            );

            renderer.clear();

            // Render Floor
            glm::mat4 floorModel = glm::mat4(1.0f);
            floorModel = glm::scale(floorModel, glm::vec3(10.0f, 1.0f, 10.0f));
            renderer.draw(floorMesh, shader, floorModel, view, projection, glm::vec3(0.4f, 0.4f, 0.4f));

            // Render Cube 1
            glm::mat4 cube1Model = glm::mat4(1.0f);
            cube1Model = glm::translate(cube1Model, glm::vec3(-2.0f, 0.5f, 0.0f));
            renderer.draw(cubeMesh, shader, cube1Model, view, projection, glm::vec3(0.8f, 0.2f, 0.2f));

            // Render Cube 2
            glm::mat4 cube2Model = glm::mat4(1.0f);
            cube2Model = glm::translate(cube2Model, glm::vec3(0.0f, 0.5f, 0.0f));
            cube2Model = glm::rotate(cube2Model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            renderer.draw(cubeMesh, shader, cube2Model, view, projection, glm::vec3(0.2f, 0.8f, 0.2f));

            // Render Cube 3
            glm::mat4 cube3Model = glm::mat4(1.0f);
            cube3Model = glm::translate(cube3Model, glm::vec3(2.0f, 0.5f, -1.0f));
            cube3Model = glm::rotate(cube3Model, glm::radians(25.0f), glm::vec3(1.0f, 1.0f, 0.0f));
            cube3Model = glm::scale(cube3Model, glm::vec3(1.5f, 1.5f, 1.5f));
            renderer.draw(cubeMesh, shader, cube3Model, view, projection, glm::vec3(0.2f, 0.2f, 0.8f));

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
