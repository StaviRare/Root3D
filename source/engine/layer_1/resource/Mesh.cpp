#include "Types.h"
#include "Mesh.h"
#include "MeshManager.h"

Mesh::Mesh()
{
    m_name = "Mesh";
}

Mesh::~Mesh()
{
    MeshManager::TryDestroy(m_resourceID);
}

void Mesh::Apply()
{
    // m_resourceID = MeshManager::CreateOrGet(name);
}

void Mesh::Load(const string& name)
{
    m_resourceID = MeshManager::CreateOrGet(name);
}

void Mesh::CreatePrimitive(PrimitiveType type)
{
    string typeName = "";

    switch(type){
    case PrimitiveType::Quad:
        typeName = "quad";
        break;
    case PrimitiveType::Cube:
        typeName = "cube";
        break;
    case PrimitiveType::Sphere:
        typeName = "sphere";
        break;
    }

    string virtualPath = MeshManager::GetVirtualPath();
    string primitivePath = virtualPath + typeName;
    m_resourceID = MeshManager::CreateOrGet(primitivePath);
}

uniqueID Mesh::getResourceID() const
{
    return m_resourceID;
}

void Mesh::SetVertices(const std::vector<Vector3>& verts)
{
    vertices = verts;
}

void Mesh::SetTexCoords(const std::vector<Vector2>& uv)
{
    texCoords = uv;
}

void Mesh::SetNormals(const std::vector<Vector3>& norms)
{
    normals = norms;
}

void Mesh::SetIndices(const std::vector<int>& inds)
{
    indices = inds;
}

const std::vector<Vector3>& Mesh::GetVertices() const
{
    return vertices;
}

const std::vector<Vector2>& Mesh::GetTexCoords() const
{
    return texCoords;
}

const std::vector<Vector3>& Mesh::GetNormals() const
{
    return normals;
}

const std::vector<int>& Mesh::GetIndices() const
{
    return indices;
}