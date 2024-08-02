#pragma once

#include "Types.h"

#define ENGINE_LOG_DEBUG 0
#define ENGINE_LOG_INFO  1
#define ENGINE_LOG_WARN  2
#define ENGINE_LOG_ERROR 3

#ifndef ENGINE_LOG_LEVEL
#define ENGINE_LOG_LEVEL 0
#endif

#if ENGINE_LOG_LEVEL <= ENGINE_LOG_DEBUG
#define ENGINE_DEBUG(message) Log::Debug(message)
#else
#define ENGINE_DEBUG(message)
#endif

#if ENGINE_LOG_LEVEL <= ENGINE_LOG_INFO
#define ENGINE_INFO(message) Log::Info(message)
#else
#define ENGINE_INFO(message)
#endif

#if ENGINE_LOG_LEVEL <= ENGINE_LOG_WARN
#define ENGINE_WARN(message) Log::Warning(message)
#else
#define ENGINE_WARN(message)
#endif

#if ENGINE_LOG_LEVEL <= ENGINE_LOG_ERROR
#define ENGINE_ERROR(message) Log::Error(message)
#else
#define ENGINE_ERROR(message)
#endif

class Log
{
    public:
    static void Debug(const string& message);
    static void Info(const string& message);
    static void Error(const string& errorMessage);
    static void Warning(const string& warningMessage);
};
