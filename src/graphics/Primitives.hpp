#pragma once

#include <array>

namespace Primitives {

    constexpr std::array<float, 108> cubeVertices = {
        // Back face
        -0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f,  
        // Front face
        -0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  
        // Left face
        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        // Right face
         0.5f,  0.5f,  0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        // Bottom face
        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f, -0.5f,  
        // Top face
        -0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f,  0.5f   
    };

    constexpr std::array<float, 18> planeVertices = {
        -0.5f,  0.0f, -0.5f,
         0.5f,  0.0f, -0.5f,
         0.5f,  0.0f,  0.5f,

        -0.5f,  0.0f, -0.5f,
         0.5f,  0.0f,  0.5f,
        -0.5f,  0.0f,  0.5f
    };

}
