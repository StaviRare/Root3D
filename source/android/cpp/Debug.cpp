#include <android/log.h>
#include "Debug.h"

#define LOG_TAG "Root3D"

Debug::Debug() {}

void Debug::Log(const string& message)
{
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s", message.c_str());
}

void Debug::LogError(const string& errorMessage)
{
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Error: %s", errorMessage.c_str());
}

void Debug::LogWarning(const string& warningMessage)
{
    __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "Warning: %s", warningMessage.c_str());
}