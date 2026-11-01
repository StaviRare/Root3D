#include "Log.h"
#include "MeshManager.h"
#include "Graphics.h"
#include "AssetLoader.h"
#include "Vector2.h"
#include "Vector3.h"

uniqueID MeshManager::m_nextID = 0;
std::unordered_map<uniqueID, Resource> MeshManager::m_meshes;

uniqueID MeshManager::GetHandle(uniqueID id)
{
    uniqueID returnValue = 0;

    auto it = m_meshes.find(id);

    if(it != m_meshes.end())
    {
        returnValue = it->second.handle;
    }

    return returnValue;
}

uniqueID MeshManager::TryDestroy(uniqueID id)
{
    uniqueID returnValue = 0;
    auto it = m_meshes.find(id);

    if(it != m_meshes.end())
    {
        if(it->second.useCount > 1)
        {
            it->second.useCount--;
            returnValue = id;
        }
        else
        {
            Graphics::DestroyMesh(it->second.handle);
            m_meshes.erase(it);
            ENGINE_DEBUG("Mesh destroyed | ID: " + std::to_string(id));
        }
    }

    return returnValue;
}

uniqueID MeshManager::CreateOrGet(const string& path)
{
    uniqueID returnValue = -1;
    bool assetPathValid = !path.empty();

    if(assetPathValid){
        string virtualPath = GetVirtualPath();
        bool isPrimitive = (path.find(virtualPath) != string::npos);

        if(isPrimitive){
            returnValue = LoadPrimitive(path);
        }
        else{
            returnValue = LoadAsset(path);
        }
    }

    return returnValue;
}

uniqueID MeshManager::LoadAsset(const string& path){
    uniqueID returnValue = -1;

    bool isNewMesh = true;
    hash hash = AssetLoader::GetAssetHash(path);

    // Check if mesh with same hash already exists
    for (const auto& it : m_meshes)
    {
        const Resource& r = it.second;

        if (r.id == hash)
        {
            returnValue = it.first;
            m_meshes[returnValue].useCount++;
            isNewMesh = false;
        }
    }

    if (isNewMesh)
    {
        auto assetData = AssetLoader::LoadResource(path);

        MeshUpload upload;

        std::vector<float> vertices;
        std::vector<float> texCoords;
        std::vector<float> normals;
        std::vector<int> indices;

        std::string content(assetData.begin(), assetData.end());
        std::istringstream file(content);

        std::string line;

        while (std::getline(file, line))
        {
            std::istringstream stream(line);
            std::string type;

            stream >> type;

            if (type == "v")
            {
                float x, y, z;
                stream >> x >> y >> z;

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
            else if (type == "vt")
            {
                float u, v;
                stream >> u >> v;

                texCoords.push_back(u);
                texCoords.push_back(v);
            }
            else if (type == "vn")
            {
                float x, y, z;
                stream >> x >> y >> z;

                normals.push_back(x);
                normals.push_back(y);
                normals.push_back(z);
            }
            else if (type == "f")
            {
                std::vector<int> face;
                std::string token;

                while (stream >> token)
                {
                    size_t slash = token.find('/');
                    int index = std::stoi(token.substr(0, slash));

                    if (index < 0)
                    {
                        index = static_cast<int>(vertices.size() / 3) + index + 1;
                    }

                    face.push_back(index - 1);
                }

                for (size_t i = 1; i + 1 < face.size(); i++)
                {
                    indices.push_back(face[0]);
                    indices.push_back(face[i]);
                    indices.push_back(face[i + 1]);
                }
            }
        }

        upload.vertices = vertices.data();
        upload.verticesSize = static_cast<unsigned int>(vertices.size());

        upload.texCoords = texCoords.data();
        upload.texCoordsSize = static_cast<unsigned int>(texCoords.size());

        upload.normals = normals.data();
        upload.normalsSize = static_cast<unsigned int>(normals.size());

        upload.indices = indices.data();
        upload.indicesSize = static_cast<unsigned int>(indices.size());

        uniqueID meshHandle = Graphics::CreateMesh(upload);

        if (meshHandle)
        {
            m_nextID++;
            Resource resource;
            resource.handle = meshHandle;
            resource.useCount = 1;
            resource.id = hash;
            m_meshes[m_nextID] = resource;
            returnValue = m_nextID;
        }
    }

    bool meshIsValid = returnValue > 0;

    if (meshIsValid)
    {
        if (isNewMesh)
        {
            ENGINE_DEBUG("Mesh created | ID: " + std::to_string(returnValue));
        }
    }

    return returnValue;
}

uniqueID MeshManager::LoadPrimitive(const string& path)
{
    uniqueID returnValue = -1;

    bool isNewMesh = true;
    hash hash = AssetLoader::GetAssetHash(path);

    // Check if mesh with same hash already exists
    for (const auto& it : m_meshes)
    {
        const Resource& r = it.second;

        if (r.id == hash)
        {
            returnValue = it.first;
            m_meshes[returnValue].useCount++;
            isNewMesh = false;
        }
    }

    if (isNewMesh){

        bool isCube = (path.find("cube") != string::npos);
        bool isQuad = (path.find("quad") != string::npos);

        std::vector<int> indices;
        std::vector<Vector3> normals;
        std::vector<Vector3> vertices;
        std::vector<Vector2> texCoords;

        if(isQuad){
            vertices =
            {
                Vector3(-0.5f, -0.5f, 0.0f),
                Vector3(0.5f, -0.5f, 0.0f),
                Vector3(0.5f, 0.5f, 0.0f),
                Vector3(-0.5f, 0.5f, 0.0f)
            };

            texCoords =
            {
                Vector2(0.0f, 1.0f),
                Vector2(1.0f, 1.0f),
                Vector2(1.0f, 0.0f),
                Vector2(0.0f, 0.0f)
            };

            normals =
            {
                Vector3(0.0f, 0.0f, 1.0f),
                Vector3(0.0f, 0.0f, 1.0f),
                Vector3(0.0f, 0.0f, 1.0f),
                Vector3(0.0f, 0.0f, 1.0f)
            };

            indices =
            {
                0, 1, 2,
                2, 3, 0
            };
        }
        if (isCube)
        {
            vertices =
            {
                // Front
                Vector3(0.5f, 0.5f, 0.5f),
                Vector3(-0.5f, 0.5f, 0.5f),
                Vector3(-0.5f, -0.5f, 0.5f),
                Vector3(0.5f, -0.5f, 0.5f),

                // Up
                Vector3(0.5f, 0.5f, 0.5f),
                Vector3(0.5f, 0.5f, -0.5f),
                Vector3(-0.5f, 0.5f, -0.5f),
                Vector3(-0.5f, 0.5f, 0.5f),

                // Right
                Vector3(0.5f, 0.5f, 0.5f),
                Vector3(0.5f, -0.5f, 0.5f),
                Vector3(0.5f, -0.5f, -0.5f),
                Vector3(0.5f, 0.5f, -0.5f),

                // Back
                Vector3(-0.5f, 0.5f, -0.5f),
                Vector3(0.5f, 0.5f, -0.5f),
                Vector3(0.5f, -0.5f, -0.5f),
                Vector3(-0.5f, -0.5f, -0.5f),

                // Down
                Vector3(-0.5f, -0.5f, 0.5f),
                Vector3(-0.5f, -0.5f, -0.5f),
                Vector3(0.5f, -0.5f, -0.5f),
                Vector3(0.5f, -0.5f, 0.5f),

                // Left
                Vector3(-0.5f, -0.5f, 0.5f),
                Vector3(-0.5f, 0.5f, 0.5f),
                Vector3(-0.5f, 0.5f, -0.5f),
                Vector3(-0.5f, -0.5f, -0.5f)
            };

            texCoords =
            {
                Vector2(1.f, 1.f), Vector2(0.f, 1.f), Vector2(0.f, 0.f), Vector2(1.f, 0.f),
                Vector2(1.f, 1.f), Vector2(0.f, 1.f), Vector2(0.f, 0.f), Vector2(1.f, 0.f),
                Vector2(1.f, 1.f), Vector2(0.f, 1.f), Vector2(0.f, 0.f), Vector2(1.f, 0.f),
                Vector2(1.f, 1.f), Vector2(0.f, 1.f), Vector2(0.f, 0.f), Vector2(1.f, 0.f),
                Vector2(1.f, 1.f), Vector2(0.f, 1.f), Vector2(0.f, 0.f), Vector2(1.f, 0.f),
                Vector2(1.f, 1.f), Vector2(0.f, 1.f), Vector2(0.f, 0.f), Vector2(1.f, 0.f)
            };

            normals =
            {
                Vector3(0.f, 0.f, 1.f), Vector3(0.f, 0.f, 1.f), Vector3(0.f, 0.f, 1.f), Vector3(0.f, 0.f, 1.f),
                Vector3(0.f, 1.f, 0.f), Vector3(0.f, 1.f, 0.f), Vector3(0.f, 1.f, 0.f), Vector3(0.f, 1.f, 0.f),
                Vector3(1.f, 0.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(1.f, 0.f, 0.f), Vector3(1.f, 0.f, 0.f),
                Vector3(0.f, 0.f, -1.f), Vector3(0.f, 0.f, -1.f), Vector3(0.f, 0.f, -1.f), Vector3(0.f, 0.f, -1.f),
                Vector3(0.f, -1.f, 0.f), Vector3(0.f, -1.f, 0.f), Vector3(0.f, -1.f, 0.f), Vector3(0.f, -1.f, 0.f),
                Vector3(-1.f, 0.f, 0.f), Vector3(-1.f, 0.f, 0.f), Vector3(-1.f, 0.f, 0.f), Vector3(-1.f, 0.f, 0.f)
            };

            indices =
            {
                0, 1, 2, 0, 2, 3,
                4, 5, 6, 4, 6, 7,
                8, 9, 10, 8, 10, 11,
                12, 13, 14, 12, 14, 15,
                16, 17, 18, 16, 18, 19,
                20, 21, 22, 20, 22, 23
            };
        }


        MeshUpload upload;
        upload.vertices = reinterpret_cast<const float*>(vertices.data());
        upload.verticesSize = static_cast<unsigned int>(vertices.size() * 3);
        upload.texCoords = reinterpret_cast<const float*>(texCoords.data());
        upload.texCoordsSize = static_cast<unsigned int>(texCoords.size() * 2);
        upload.normals = reinterpret_cast<const float*>(normals.data());
        upload.normalsSize = static_cast<unsigned int>(normals.size() * 3);
        upload.indices = indices.data();
        upload.indicesSize = static_cast<unsigned int>(indices.size());

        uniqueID meshHandle = Graphics::CreateMesh(upload);

        if (meshHandle)
        {
            m_nextID++;
            Resource resource;
            resource.handle = meshHandle;
            resource.useCount = 1;
            resource.id = hash;
            m_meshes[m_nextID] = resource;
            returnValue = m_nextID;
        }
    }

    bool meshIsValid = returnValue > 0;

    if (meshIsValid)
    {
        if (isNewMesh)
        {
            ENGINE_DEBUG("Mesh created | ID: " + std::to_string(returnValue));
        }
    }

    return returnValue;
}

string MeshManager::GetVirtualPath()
{
    return "primitive://";
}