#include "Shader.hpp"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

Shader::Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    programId_ = glCreateProgram();
    glAttachShader(programId_, vertexShader);
    glAttachShader(programId_, fragmentShader);
    glLinkProgram(programId_);

    int success;
    glGetProgramiv(programId_, GL_LINK_STATUS, &success);
    if (!success) {
        int length;
        glGetProgramiv(programId_, GL_INFO_LOG_LENGTH, &length);
        std::string infoLog(length, ' ');
        glGetProgramInfoLog(programId_, length, nullptr, infoLog.data());
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(programId_);
        programId_ = 0;
        
        std::cerr << "Shader Linking failed:\n" << infoLog << "\n";
        throw std::runtime_error("Failed to link shader program.");
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    if (programId_ != 0) {
        glDeleteProgram(programId_);
    }
}

void Shader::use() const {
    glUseProgram(programId_);
}

void Shader::setMat4(const char* name, const glm::mat4& matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(programId_, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setVec3(const char* name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(programId_, name), 1, glm::value_ptr(value));
}

std::string Shader::readFile(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::string errorMsg = "Failed to open shader file: " + filepath.string();
        std::cerr << errorMsg << "\n";
        throw std::runtime_error(errorMsg);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int Shader::compileShader(unsigned int shaderType, const std::string& source) {
    unsigned int shader = glCreateShader(shaderType);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::string infoLog(length, ' ');
        glGetShaderInfoLog(shader, length, nullptr, infoLog.data());
        glDeleteShader(shader);
        
        std::cerr << "Shader Compilation failed for type " << shaderType << ":\n" << infoLog << "\n";
        throw std::runtime_error("Failed to compile shader.");
    }
    
    return shader;
}
