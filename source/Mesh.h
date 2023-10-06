#ifndef MESH_H
#define MESH_H

#include <vector>

class Mesh {
public:
    std::vector<int>& GetIndices();
    std::vector<float>& GetVertices();

    void SetIndices(const std::vector<int>& triangles);
    void SetVertices(const std::vector<float>& vertices);

private:
    std::vector<int> indices;
    std::vector<float> vertices;
};

#endif
