#include "Renderer.hpp"
#include <glad/gl.h>

Renderer::Renderer() {
    glEnable(GL_DEPTH_TEST);
}

void Renderer::clear() const {
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw(
    const Mesh& mesh,
    const Shader& shader,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& color) const {
    
    shader.use();
    shader.setMat4("uModel", model);
    shader.setMat4("uView", view);
    shader.setMat4("uProjection", projection);
    shader.setVec3("uColor", color);

    mesh.draw();
}
