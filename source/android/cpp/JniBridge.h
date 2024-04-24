#pragma once

#include <jni.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>

class JniBridge
{
    public:
    static void Initialize(JNIEnv* env, jobject surface, jobject assetManagerObj);
    static void Uninitialize();
    static ANativeWindow* GetNativeWindow();
    static AAssetManager* GetAssetManager();

    private:
    static ANativeWindow* window;
    static AAssetManager* assetManager;
};
