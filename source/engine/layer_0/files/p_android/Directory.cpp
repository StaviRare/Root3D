#include <android/asset_manager.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/stat.h>

#include "JniBridge.h"
#include "Directory.h"
#include "Log.h"
#include "File.h"

void Directory::Create(const string& path)
{
    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

bool Directory::Exists(const string& path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

// ToDo! - Delete resource file after play.
string Directory::GetResourcePath()
{
    const char* filename = "resources.bin";
    auto mgr = JniBridge::GetAssetManager();

    if (!mgr)
    {
        ENGINE_ERROR("Asset manager not initialized");
        return "Asset manager not initialized.";
    }

    AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_BUFFER);
    if (asset == nullptr)
    {
        ENGINE_ERROR("Failed to open asset: " + string(filename));
        return "Failed to open asset.";
    }

    // Get the size of the asset
    off_t assetLength = AAsset_getLength(asset);
    if (assetLength == 0)
    {
        ENGINE_ERROR("Asset '" + string(filename) + "' is empty or not readable");
        AAsset_close(asset);
        return "Asset is empty or not readable.";
    }

    // Read asset contents into memory
    std::vector<char> buffer(assetLength);
    AAsset_read(asset, buffer.data(), assetLength);
    AAsset_close(asset);

    // Determine a writable directory for saving the asset
    string writableDir = "/data/data/com.example.root3d/files/";

    // Check if the directory exists, if not, create it
    if (!Directory::Exists(writableDir))
    {
        Directory::Create(writableDir);
        ENGINE_ERROR("Asset directory created: " + writableDir);
    }
    else
    {
        ENGINE_WARN("Asset directory already exists: " + writableDir);
    }

    // Create a file path for saving the asset
    string filePath = writableDir + filename;

    // Save asset contents to file
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile)
    {
        ENGINE_ERROR("Failed to create file for saving asset: " + filePath);
        ENGINE_ERROR("Details: " + string(std::strerror(errno)));
        return "Failed to create file for saving asset.";
    }
    outFile.write(buffer.data(), assetLength);
    outFile.close();

    ENGINE_INFO("Successfully saved asset to file: " + filePath);

    return filePath;
}
