#pragma once

#include <vector>
#include "Vector2.h"
#include "Vector3.h"

class Mesh
{
    public:

    void SetVertices(const std::vector<Vector3>& verts)
    {
        vertices = verts;
    }

    void SetTexCoords(const std::vector<Vector2>& uv)
    {
        texCoords = uv;
    }

    void SetNormals(const std::vector<Vector3>& norms)
    {
        normals = norms;
    }

    void SetIndices(const std::vector<int>& inds)
    {
        indices = inds;
    }

    const std::vector<Vector3>& GetVertices() const
    {
        return vertices;
    }

    const std::vector<Vector2>& GetTexCoords() const
    {
        return texCoords;
    }

    const std::vector<Vector3>& GetNormals() const
    {
        return normals;
    }

    const std::vector<int>& GetIndices() const
    {
        return indices;
    }

    private:
    std::vector<Vector3> vertices;
    std::vector<Vector2> texCoords;
    std::vector<Vector3> normals;
    std::vector<int> indices;
};
