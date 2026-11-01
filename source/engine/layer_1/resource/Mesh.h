#pragma once

#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "Object.h"

enum class PrimitiveType
{
    Cube,
    Sphere,
    Quad
};

class Mesh : public Object
{
    public:
    Mesh();
    ~Mesh();
    void Apply();
    void Load(const string& name);
    void CreatePrimitive(PrimitiveType type);
    uniqueID getResourceID() const;
    void SetVertices(const std::vector<Vector3>& verts);
    void SetTexCoords(const std::vector<Vector2>& uv);
    void SetNormals(const std::vector<Vector3>& norms);
    void SetIndices(const std::vector<int>& inds);
    const std::vector<Vector3>& GetVertices() const;
    const std::vector<Vector2>& GetTexCoords() const;
    const std::vector<Vector3>& GetNormals() const;
    const std::vector<int>& GetIndices() const;

    private:
    uniqueID m_resourceID = 0;
    std::vector<int> indices;
    std::vector<Vector3> normals;
    std::vector<Vector3> vertices;
    std::vector<Vector2> texCoords;
};
