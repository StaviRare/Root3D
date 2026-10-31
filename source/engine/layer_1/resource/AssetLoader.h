#pragma once

#include <vector>
#include "Font.h"
#include "Types.h"

class AssetLoader
{
    public:
    static Font LoadFont(const std::string& path);
    static hash GetAssetHash(const string& path);
    static std::vector<char> LoadResource(const string& resourcePath);
};
