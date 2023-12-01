#pragma once

#include <vector>

class Mesh {
public:
    std::vector<int>& GetIndices()
    {
        return indices;
    }

    std::vector<float>& GetVertices()
    {
        return vertices;
    }

    void SetIndices(const std::vector<int>& newIndices)
    {
        indices = newIndices;
    }

    void SetVertices(const std::vector<float>& newVertices)
    {
        vertices = newVertices;
    }

private:
    std::vector<int> indices;
    std::vector<float> vertices;
};
