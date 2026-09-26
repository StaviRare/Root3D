#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <cstring>

#include "Log.h"
#include "Directory.h"
#include "File.h"

void Directory::Create(const string& path)
{
    if (mkdir(path.c_str(), 0755) != 0 && errno != EEXIST)
    {
        ENGINE_ERROR("Failed to create directory: " + path);
    }
}

bool Directory::Exists(const string& path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

string Directory::GetResourcePath()
{
    string returnValue = "";
    char buffer[PATH_MAX];
    ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);

    if (length == -1)
    {
        ENGINE_ERROR("Failed to get executable path");
    }
    else
    {
        buffer[length] = '\0';
        string executablePath(buffer);
        string::size_type pos = executablePath.find_last_of('/');
        returnValue = executablePath.substr(0, pos) + "/resources.bin";
    }

    return returnValue;
}