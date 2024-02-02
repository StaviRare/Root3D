#pragma once

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>

class FileManager {
public:
    static bool write(const std::string& filePath, const std::string& data);
    static std::string read(const std::string& filePath);
};

#endif
