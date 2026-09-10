#pragma once

#include <cstddef>
#include <span>

class Mesh {
public:
    explicit Mesh(std::span<const float> vertices);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;

private:
    unsigned int vao_{0};
    unsigned int vbo_{0};
    int vertexCount_{0};
};
