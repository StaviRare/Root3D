#include "JniBridge.h"
#include "Debug.h"
#include "Core.h"

ANativeWindow* JniBridge::window = nullptr;
AAssetManager* JniBridge::assetManager = nullptr;

void JniBridge::Initialize(JNIEnv* env, jobject surface, jobject assetManagerObj)
{
    bool canInitialize = true;

    if (surface) // Fixes issue when changing orientation
    {
        if (window)
        {
            ANativeWindow_release(window);
        }

        window = ANativeWindow_fromSurface(env, surface);
        assetManager = AAssetManager_fromJava(env, assetManagerObj);

        if (assetManager)
        {
            Debug::Log("Asset manager initialized");
        }
        else
        {
            canInitialize = false;
            Debug::LogError("Asset manager was no initialized!");
        }

        if (window)
        {
            Debug::Log("Surface set and renderer initialized");
        }
        else
        {
            canInitialize = false;
            Debug::LogError("Surface was no initialized!");
        }

        if (canInitialize)
        {
            Core::Initialize();
        }
    }
}

void JniBridge::Uninitialize()
{
    Debug::Log("Uninitializing");

    Core::UnInitialize();

    if (window)
    {
        ANativeWindow_release(window);
        window = nullptr;
    }
}

ANativeWindow* JniBridge::GetNativeWindow()
{
    return window;
}

AAssetManager* JniBridge::GetAssetManager()
{
    return assetManager;
}

extern "C"
{
    JNIEXPORT void JNICALL Java_com_example_root3d_MainActivity_nativeOnStart(JNIEnv* env, jobject obj)
    {
        Debug::Log("OnNativeStart");
    }

    JNIEXPORT void JNICALL Java_com_example_root3d_MainActivity_nativeOnResume(JNIEnv* env, jobject obj)
    {
        Debug::Log("OnNativeResume");
    }

    JNIEXPORT void JNICALL Java_com_example_root3d_MainActivity_nativeOnPause(JNIEnv* env, jobject obj)
    {
        Debug::Log("OnNativePause");
    }

    JNIEXPORT void JNICALL Java_com_example_root3d_MainActivity_nativeOnStop(JNIEnv* env, jobject obj)
    {
        Debug::Log("OnNativeStop");
    }

    JNIEXPORT void JNICALL Java_com_example_root3d_MainActivity_nativeInitialize(JNIEnv* env, jobject obj, jobject surface, jobject assetManager)
    {
        JniBridge::Initialize(env, surface, assetManager);
    }

    JNIEXPORT void JNICALL Java_com_example_root3d_MainActivity_nativeTick(JNIEnv* env, jobject obj)
    {
        Core::Tick();
    }

    JNIEXPORT void JNICALL Java_com_example_root3d_MainActivity_nativeUninitialize(JNIEnv* env, jobject obj)
    {
        JniBridge::Uninitialize();
    }
}
