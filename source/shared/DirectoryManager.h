#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <string>

class DirectoryManager {
public:
    static bool create(const std::string& dirPath);
    static bool exists(const std::string& dirPath);
};

#endif
