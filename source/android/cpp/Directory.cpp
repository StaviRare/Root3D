#include "Directory.h"
#include "Debug.h"
#include "File.h"
#include <android/asset_manager.h>
#include "JniBridge.h"
#include <fstream>
#include <sstream>
#include <vector>

void Directory::Create(const string& path)
{
    // ToDo - Implement
}

bool Directory::Exists(const string& path)
{
    // ToDo - Implement
    return false;
}

// ToDo! - Delete resource file after play.
string Directory::GetResourcePath()
{
    const char* filename = "resources.bin";
    auto mgr = JniBridge::GetAssetManager();

    if (!mgr)
    {
        Debug::LogError("Asset manager not initialized");
        return "Asset manager not initialized.";
    }

    AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_BUFFER);
    if (asset == nullptr)
    {
        Debug::LogError("Failed to open asset: " + std::string(filename));
        return "Failed to open asset.";
    }

    // Get the size of the asset
    off_t assetLength = AAsset_getLength(asset);
    if (assetLength == 0)
    {
        Debug::LogError("Asset '" + std::string(filename) + "' is empty or not readable");
        AAsset_close(asset);
        return "Asset is empty or not readable.";
    }

    // Read asset contents into memory
    std::vector<char> buffer(assetLength);
    AAsset_read(asset, buffer.data(), assetLength);
    AAsset_close(asset);

    // Determine a writable directory for saving the asset
    std::string writableDir = "/data/data/com.example.root3d/files/"; // Example directory, adjust as needed

    // Create a file path for saving the asset
    std::string filePath = writableDir + filename;

    // Save asset contents to file
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile)
    {
        Debug::LogError("Failed to create file for saving asset: " + filePath);
        return "Failed to create file for saving asset.";
    }
    outFile.write(buffer.data(), assetLength);
    outFile.close();

    Debug::Log("Successfully saved asset to file: " + filePath);

    return filePath;
}
