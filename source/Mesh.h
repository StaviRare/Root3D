#pragma once

#include <vector>

class Mesh {
public:
    void SetVertices(const std::vector<float>& verts) 
    {
        vertices = verts;
    }
    void SetTexCoords(const std::vector<float>& texCoords) 
    {
        this->texCoords = texCoords;
    }
    void SetNormals(const std::vector<float>& norms) 
    {
        normals = norms;
    }
    void SetIndices(const std::vector<int>& inds)
    {
        indices = inds;
    }

    const std::vector<float>& GetVertices() const 
    {
        return vertices;
    }
    const std::vector<float>& GetTexCoords() const 
    {
        return texCoords;
    }
    const std::vector<float>& GetNormals() const 
    {
        return normals;
    }
    const std::vector<int>& GetIndices() const 
    {
        return indices;
    }

private:
    std::vector<float> vertices;
    std::vector<float> texCoords;
    std::vector<float> normals;
    std::vector<int> indices;
};
