#include <windows.h>
#include "Directory.h"
#include "Debug.h"

void Directory::Create(const std::string& path)
{
    std::wstring stemp = std::wstring(path.begin(), path.end());
    LPCWSTR sw = stemp.c_str();

    if (!CreateDirectory(sw, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
    {
        Debug::LogError("Failed to create directory: " + path);
    }
}

bool Directory::Exists(const std::string& path)
{
    std::wstring stemp = std::wstring(path.begin(), path.end());
    LPCWSTR sw = stemp.c_str();
    DWORD dwAttrib = GetFileAttributes(sw);

    return ( dwAttrib != INVALID_FILE_ATTRIBUTES && ( dwAttrib & FILE_ATTRIBUTE_DIRECTORY ) );
}

string Directory::GetResourcePath()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    string::size_type pos = string(buffer).find_last_of("\\/");
    string returnValue = string(buffer).substr(0, pos) + "/resources.bin"; // Temporary solution. Will need to be changed.

    // ToDo! - Check if file exists.

    return returnValue;
}
