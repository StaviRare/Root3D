#include <fstream>

#include "Log.h"
#include "File.h"

bool File::Exists(const string& filePath)
{
    std::ifstream file(filePath);
    bool returnValue = file.good();
    file.close();

    return returnValue;
}

bool File::Delete(const std::string& filePath)
{
    bool returnValue = true;

    if (remove(filePath.c_str()) != 0)
    {
        ENGINE_ERROR("Failed to delete file: " + filePath);

        returnValue = false;
    }

    return returnValue;
}

string File::ReadAllText(const string& filePath)
{
    string returnValue = "";

    std::ifstream file(filePath);

    if (file.is_open())
    {
        string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        returnValue = content;
    }
    else
    {
        ENGINE_ERROR("Failed to open file for reading: " + filePath);
    }

    return returnValue;
}

void File::WriteAllText(const string& filePath, const string& data)
{
    std::ofstream file(filePath);
    
    if (file.is_open())
    {
        file << data;
    }
    else
    {
        ENGINE_ERROR("Failed to open file for writing: " + filePath);
    }
}
