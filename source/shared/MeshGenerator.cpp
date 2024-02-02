#include "MeshGenerator.h"
#include "Math.h"

// USE VECTOR3 HERE

Mesh MeshGenerator::GetQuad() {
    Mesh quadMesh;
    // Define vertices, texture coordinates, normals, and indices for a quad
    std::vector<float> vertices = {
        // positions
        -0.5f, -0.5f, 0.0f, // bottom left
         0.5f, -0.5f, 0.0f, // bottom right
         0.5f,  0.5f, 0.0f, // top right
        -0.5f,  0.5f, 0.0f  // top left 
    };

    std::vector<float> texCoords = {
        // texture coordinates (inverted y-coordinate)
        0.0f, 1.0f, // bottom left
        1.0f, 1.0f, // bottom right
        1.0f, 0.0f, // top right
        0.0f, 0.0f  // top left
    };

    std::vector<float> normals = {
        // normals (facing out of the screen)
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };

    std::vector<int> indices = {
        // indices (two triangles)
        0, 1, 2, // first triangle
        2, 3, 0  // second triangle
    };

    quadMesh.SetVertices(vertices);
    quadMesh.SetTexCoords(texCoords);
    quadMesh.SetNormals(normals);
    quadMesh.SetIndices(indices);

    return quadMesh;
}

Mesh MeshGenerator::GetCube() {
    Mesh cubeMesh;

    // Define vertices (positions) for a cube
    std::vector<float> vertices = {
        // Front face
        -0.5f, -0.5f,  0.5f, // Bottom-left
         0.5f, -0.5f,  0.5f, // Bottom-right
         0.5f,  0.5f,  0.5f, // Top-right
        -0.5f,  0.5f,  0.5f, // Top-left

        // Back face
        -0.5f, -0.5f, -0.5f, // Bottom-left
         0.5f, -0.5f, -0.5f, // Bottom-right
         0.5f,  0.5f, -0.5f, // Top-right
        -0.5f,  0.5f, -0.5f, // Top-left

        // Left face
        -0.5f, -0.5f, -0.5f, // Bottom-left
        -0.5f, -0.5f,  0.5f, // Bottom-right
        -0.5f,  0.5f,  0.5f, // Top-right
        -0.5f,  0.5f, -0.5f, // Top-left

        // Right face
         0.5f, -0.5f, -0.5f, // Bottom-left
         0.5f, -0.5f,  0.5f, // Bottom-right
         0.5f,  0.5f,  0.5f, // Top-right
         0.5f,  0.5f, -0.5f, // Top-left

        // Top face
        -0.5f,  0.5f, -0.5f, // Bottom-left
         0.5f,  0.5f, -0.5f, // Bottom-right
         0.5f,  0.5f,  0.5f, // Top-right
        -0.5f,  0.5f,  0.5f, // Top-left

        // Bottom face
        -0.5f, -0.5f, -0.5f, // Bottom-left
         0.5f, -0.5f, -0.5f, // Bottom-right
         0.5f, -0.5f,  0.5f, // Top-right
        -0.5f, -0.5f,  0.5f  // Top-left
    };

    // Define texture coordinates for a cube
    std::vector<float> texCoords = {
        // Texture coordinates for each face (6 faces, 4 vertices each, 2 coordinates per vertex)
        // Front face
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        // Back face
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        // Left face
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        // Right face
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        // Top face
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        // Bottom face
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };

    // Define normals for a cube
    std::vector<float> normals = {
        // Normals for each face (6 faces, 4 vertices each, 3 components per normal)
        // Front face
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        // Back face
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,

        // Left face
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,

        // Right face
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        // Top face
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        // Bottom face
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f
    };

    // Define indices for a cube
    std::vector<int> indices = {
        // indices for each face (6 faces, 2 triangles per face, 3 indices per triangle)
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        8, 9, 10, 10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Top face
        16, 17, 18, 18, 19, 16,
        // Bottom face
        20, 21, 22, 22, 23, 20
    };

    // Set the separated attributes for the mesh
    cubeMesh.SetVertices(vertices);
    cubeMesh.SetTexCoords(texCoords);
    cubeMesh.SetNormals(normals);
    cubeMesh.SetIndices(indices);

    return cubeMesh;
}

Mesh MeshGenerator::GetSphere() {
    Mesh sphereMesh;
    float radius = 0.5f;
    unsigned int longitudeBands = 18; 
    unsigned int latitudeBands = 18;

    std::vector<float> vertices;
    std::vector<float> texCoords;
    std::vector<float> normals;
    std::vector<int> indices;

    for (unsigned int lat = 0; lat <= latitudeBands; ++lat) {
        float theta = lat * Math::PI  / latitudeBands;
        float sinTheta = Math::Sin(theta);
        float cosTheta = Math::Cos(theta);

        for (unsigned int lon = 0; lon <= longitudeBands; ++lon) {
            float phi = (lon * 2 * Math::PI / longitudeBands) - Math::PI / 2;
            float sinPhi = Math::Sin(phi);
            float cosPhi = Math::Cos(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;
            float u = 1 - (float(lon) / longitudeBands);
            float v = (float(lat) / latitudeBands);

            vertices.push_back(radius * x);
            vertices.push_back(radius * y);
            vertices.push_back(radius * z);

            texCoords.push_back(u);
            texCoords.push_back(v);

            normals.push_back(x);
            normals.push_back(y);
            normals.push_back(z);
        }
    }

    for (unsigned int lat = 0; lat < latitudeBands; ++lat) {
        for (unsigned int lon = 0; lon < longitudeBands; ++lon) {
            int first = (lat * (longitudeBands + 1)) + lon;
            int second = first + longitudeBands + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    sphereMesh.SetVertices(vertices);
    sphereMesh.SetTexCoords(texCoords);
    sphereMesh.SetNormals(normals);
    sphereMesh.SetIndices(indices);

    return sphereMesh;
}