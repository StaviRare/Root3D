#include "FileManager.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

bool FileManager::write(const std::string& filePath, const std::string& data) {
    try {
        std::ofstream file(filePath);
        if (file.is_open()) {
            file << data;
            return true;
        }
        return false;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error writing to file: " << ex.what() << std::endl;
        return false;
    }
}

std::string FileManager::read(const std::string& filePath) {
    try {
        std::ifstream file(filePath);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
            return content;
        }
        throw std::runtime_error("Error reading file");
    }
    catch (const std::exception& ex) {
        std::cerr << "Error reading file: " << ex.what() << std::endl;
        return "";
    }
}
