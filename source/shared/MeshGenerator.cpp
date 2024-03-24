#include "MeshGenerator.h"
#include "Math.h"
#include "Vector2.h"
#include "Vector3.h"

Mesh MeshGenerator::GetQuad() 
{
    Mesh quadMesh;

    std::vector<Vector3> vertices = 
    {
        Vector3(-0.5f, -0.5f, 0.0f),
        Vector3(0.5f, -0.5f, 0.0f),
        Vector3(0.5f, 0.5f, 0.0f),
        Vector3(-0.5f, 0.5f, 0.0f)
    };

    std::vector<Vector2> texCoords = 
    {
        Vector2(0.0f, 1.0f),
        Vector2(1.0f, 1.0f),
        Vector2(1.0f, 0.0f),
        Vector2(0.0f, 0.0f)
    };

    std::vector<Vector3> normals = 
    {
        Vector3(0.0f, 0.0f, 1.0f),
        Vector3(0.0f, 0.0f, 1.0f),
        Vector3(0.0f, 0.0f, 1.0f),
        Vector3(0.0f, 0.0f, 1.0f)
    };

    std::vector<int> indices = 
    {
        0, 1, 2,
        2, 3, 0
    };

    quadMesh.SetVertices(vertices);
    quadMesh.SetTexCoords(texCoords);
    quadMesh.SetNormals(normals);
    quadMesh.SetIndices(indices);

    return quadMesh;
}

Mesh MeshGenerator::GetCube() 
{
    Mesh cubeMesh;

    std::vector<Vector3> vertices = 
    {
        // Front face
        Vector3(0.5f, 0.5f, 0.5f), 
        Vector3(-0.5f, 0.5f, 0.5f), 
        Vector3(-0.5f, -0.5f, 0.5f), 
        Vector3(0.5f, -0.5f, 0.5f),
        // Up face
        Vector3(0.5f, 0.5f, 0.5f), 
        Vector3(0.5f, 0.5f, -0.5f), 
        Vector3(-0.5f, 0.5f, -0.5f), 
        Vector3(-0.5f, 0.5f, 0.5f),
        // Right face
        Vector3(0.5f, 0.5f, 0.5f), 
        Vector3(0.5f, -0.5f, 0.5f), 
        Vector3(0.5f, -0.5f, -0.5f), 
        Vector3(0.5f, 0.5f, -0.5f),
        // Back face
        Vector3(-0.5f, 0.5f, -0.5f), 
        Vector3(0.5f, 0.5f, -0.5f), 
        Vector3(0.5f, -0.5f, -0.5f), 
        Vector3(-0.5f, -0.5f, -0.5f),
        // Down face
        Vector3(-0.5f, -0.5f, 0.5f), 
        Vector3(-0.5f, -0.5f, -0.5f), 
        Vector3(0.5f, -0.5f, -0.5f), 
        Vector3(0.5f, -0.5f, 0.5f),
        // Left face
        Vector3(-0.5f, -0.5f, 0.5f),
        Vector3(-0.5f, 0.5f, 0.5f), 
        Vector3(-0.5f, 0.5f, -0.5f), 
        Vector3(-0.5f, -0.5f, -0.5f),
    };

    std::vector<Vector2> texCoords = 
    {
        // Front face
        Vector2(1.f, 1.f), 
        Vector2(0.f, 1.f), 
        Vector2(0.f, 0.f), 
        Vector2(1.f, 0.f),
        // Up face
        Vector2(1.f, 1.f), 
        Vector2(0.f, 1.f), 
        Vector2(0.f, 0.f), 
        Vector2(1.f, 0.f),
        // Right face
        Vector2(1.f, 1.f), 
        Vector2(0.f, 1.f), 
        Vector2(0.f, 0.f), 
        Vector2(1.f, 0.f),
        // Back face
        Vector2(1.f, 1.f), 
        Vector2(0.f, 1.f), 
        Vector2(0.f, 0.f), 
        Vector2(1.f, 0.f),
        // Down face
        Vector2(1.f, 1.f), 
        Vector2(0.f, 1.f), 
        Vector2(0.f, 0.f), 
        Vector2(1.f, 0.f),
        // Left face
        Vector2(1.f, 1.f), 
        Vector2(0.f, 1.f), 
        Vector2(0.f, 0.f), 
        Vector2(1.f, 0.f),
    };

    std::vector<Vector3> normals = 
    {
        // Front face normals
        Vector3(0.0f, 0.0f, 1.0f), 
        Vector3(0.0f, 0.0f, 1.0f), 
        Vector3(0.0f, 0.0f, 1.0f), 
        Vector3(0.0f, 0.0f, 1.0f),
        // Up face normals
        Vector3(0.0f, 1.0f, 0.0f), 
        Vector3(0.0f, 1.0f, 0.0f), 
        Vector3(0.0f, 1.0f, 0.0f), 
        Vector3(0.0f, 1.0f, 0.0f),
        // Right face normals
        Vector3(1.0f, 0.0f, 0.0f), 
        Vector3(1.0f, 0.0f, 0.0f), 
        Vector3(1.0f, 0.0f, 0.0f), 
        Vector3(1.0f, 0.0f, 0.0f),
        // Back face normals
        Vector3(0.0f, 0.0f, -1.0f), 
        Vector3(0.0f, 0.0f, -1.0f), 
        Vector3(0.0f, 0.0f, -1.0f), 
        Vector3(0.0f, 0.0f, -1.0f),
        // Down face normals
        Vector3(0.0f, -1.0f, 0.0f), 
        Vector3(0.0f, -1.0f, 0.0f), 
        Vector3(0.0f, -1.0f, 0.0f), 
        Vector3(0.0f, -1.0f, 0.0f),
        // Left face normals
        Vector3(-1.0f, 0.0f, 0.0f), 
        Vector3(-1.0f, 0.0f, 0.0f), 
        Vector3(-1.0f, 0.0f, 0.0f), 
        Vector3(-1.0f, 0.0f, 0.0f),
    };

    std::vector<int> indices = 
    {
        // Front face
        0, 1, 2, 0, 2, 3,
        // Up face
        4, 5, 6, 4, 6, 7,
        // Right face
        8, 9, 10, 8, 10, 11,
        // Back face
        12, 13, 14, 12, 14, 15,
        // Down face
        16, 17, 18, 16, 18, 19,
        // Left face
        20, 21, 22, 20, 22, 23,
    };

    cubeMesh.SetVertices(vertices);
    cubeMesh.SetTexCoords(texCoords);
    cubeMesh.SetNormals(normals);
    cubeMesh.SetIndices(indices);

    return cubeMesh;
}

Mesh MeshGenerator::GetSphere()
{
    Mesh sphereMesh;
    float radius = 0.5f;
    unsigned int longitudeBands = 18;
    unsigned int latitudeBands = 18;

    std::vector<Vector3> vertices;
    std::vector<Vector2> texCoords;
    std::vector<Vector3> normals;
    std::vector<int> indices;

    for (unsigned int lat = 0; lat <= latitudeBands; ++lat)
    {
        float theta = lat * Math::PI / latitudeBands;
        float sinTheta = Math::Sin(theta);
        float cosTheta = Math::Cos(theta);

        for (unsigned int lon = 0; lon <= longitudeBands; ++lon)
        {
            float phi = lon * 2 * Math::PI / longitudeBands - Math::PI / 2;
            float sinPhi = Math::Sin(phi);
            float cosPhi = Math::Cos(phi);

            Vector3 position(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
            Vector2 uv(1 - ( float(lon) / longitudeBands ), float(lat) / latitudeBands);
            Vector3 normal = position.normalized();

            vertices.push_back(position * radius);
            texCoords.push_back(uv);
            normals.push_back(normal);
        }
    }

    for (unsigned int lat = 0; lat < latitudeBands; ++lat)
    {
        for (unsigned int lon = 0; lon < longitudeBands; ++lon)
        {
            int first = ( lat * ( longitudeBands + 1 ) ) + lon;
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