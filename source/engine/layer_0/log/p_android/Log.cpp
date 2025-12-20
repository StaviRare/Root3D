#include <android/log.h>
#include "Log.h"

#define LOG_TAG "Root3D"

void Log::Debug(const string& message)
{
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s", message.c_str());
}

void Log::Info(const string& message)
{
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", message.c_str());
}

void Log::Error(const string& message)
{
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", message.c_str());
}

void Log::Warning(const string& message)
{
    __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "%s", message.c_str());
}