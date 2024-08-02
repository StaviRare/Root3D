#include <windows.h>

#include "Log.h"
#include "Directory.h"
#include "File.h"

void Directory::Create(const string& path)
{
    std::wstring stemp = std::wstring(path.begin(), path.end());
    LPCWSTR sw = stemp.c_str();

    if (!CreateDirectory(sw, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
    {
        ENGINE_ERROR("Failed to create directory: " + path);
    }
}

bool Directory::Exists(const string& path)
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
    string resourcePath = string(buffer).substr(0, pos) + "/resources.bin"; // Temporary solution. Will need to change.

    return resourcePath;
}
