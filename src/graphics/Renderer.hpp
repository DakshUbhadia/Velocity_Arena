#pragma once

#include "Mesh.hpp"
#include "Shader.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <cstdint>

struct RendererStats {
    std::uint64_t drawCalls{0};
};


class Renderer {
public:
    Renderer();

    void beginFrame();
    void clear() const;
    
    const RendererStats& stats() const { return stats_; }

    void draw(
        const Mesh& mesh,
        const Shader& shader,
        const glm::mat4& model,
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& color);

private:
    RendererStats stats_;
};
