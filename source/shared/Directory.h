#pragma once

#include "Types.h"

class Directory
{
    public:
    static void Create(const string& path);
    static bool Exists(const string& path);
    static string GetResourcePath();
};