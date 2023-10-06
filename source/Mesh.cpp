#include "Mesh.h"

std::vector<int>& Mesh::GetIndices() {
    return indices;
}

std::vector<float>& Mesh::GetVertices() {
    return vertices;
}

void Mesh::SetIndices(const std::vector<int>& triangles) {
    this->indices = triangles;
}

void Mesh::SetVertices(const std::vector<float>& vertices) {
    this->vertices = vertices;
}