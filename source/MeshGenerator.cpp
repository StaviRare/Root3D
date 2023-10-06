#include "MeshGenerator.h"

Mesh MeshGenerator::GetQuad() {
    Mesh mesh;

    std::vector<float> vertices = {
        -0.5f,  0.5f, 0.0f,    0.0f, 0.0f,
         0.5f,  0.5f, 0.0f,    1.0f, 0.0f,
         0.5f, -0.5f, 0.0f,    1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,    0.0f, 1.0f
    };

    std::vector<int> indices = {
        0, 1, 2,
        0, 2, 3
    };

    mesh.SetVertices(vertices);
    mesh.SetIndices(indices);

    return mesh;
}

Mesh MeshGenerator::GetCube() {
    Mesh mesh;

    std::vector<float> vertices = {
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        // Back face
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        // Left face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        // Right face
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
        // Bottom face
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f
    };

    std::vector<int> indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        // Back face
        4, 5, 6,
        6, 7, 4,
        // Left face
        8, 9, 10,
        10, 11, 8,
        // Right face
        12, 13, 14,
        14, 15, 12,
        // Top face
        16, 17, 18,
        18, 19, 16,
        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    mesh.SetVertices(vertices);
    mesh.SetIndices(indices);

    return mesh;
}