#pragma once

#include <filesystem>
#include <string>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Shader {
public:
    Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    ~Shader();

    // Prevent copying because Shader owns the OpenGL program
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const;

    void setMat4(const char* name, const glm::mat4& matrix) const;
    void setVec3(const char* name, const glm::vec3& value) const;

private:
    unsigned int programId_{0};

    unsigned int compileShader(unsigned int shaderType, const std::string& source);
    std::string readFile(const std::filesystem::path& filepath);
};
