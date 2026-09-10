#pragma once

#include "Mesh.hpp"
#include "Shader.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Renderer {
public:
    Renderer();

    void clear() const;

    void draw(
        const Mesh& mesh,
        const Shader& shader,
        const glm::mat4& model,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& color) const;
};
