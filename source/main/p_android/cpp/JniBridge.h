#pragma once

#include <jni.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include "Runtime.h"

class JniBridge
{
    public:
    static void SetSurface(JNIEnv* env, jobject surface);
    static void SetAssetManager(JNIEnv* env, jobject assetManagerObj);
    static void Initialize();
    static void UnInitialize();
    static void Resume();
    static void Pause();
    static void Tick();
    static void Resize(int width, int height);
    static ANativeWindow* GetNativeWindow();
    static AAssetManager* GetAssetManager();

    private:
    static ANativeWindow* window;
    static AAssetManager* assetManager;
    static Runtime* runtime;
};
