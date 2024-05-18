#pragma once

#include "Types.h"

class File
{
    public:
    static bool Exists(const string& filePath);
    static bool Delete(const string& filePath);
    static string ReadAllText(const string& filePath);
    static void WriteAllText(const string& filePath, const string& data);
};