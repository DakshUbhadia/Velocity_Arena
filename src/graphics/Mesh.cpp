#include "Mesh.hpp"
#include <glad/gl.h>

Mesh::Mesh(std::span<const float> vertices) {
    vertexCount_ = static_cast<int>(vertices.size() / 3);

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }
}

void Mesh::draw() const {
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
    // No need to unbind VAO here, next draw will bind its own. Unbinding costs performance unnecessarily.
}
