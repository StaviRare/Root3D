#include "JniBridge.h"
#include "Debug.h"
#include "Core.h"
#include "Screen.h"

ANativeWindow* JniBridge::window = nullptr;
AAssetManager* JniBridge::assetManager = nullptr;

void JniBridge::SetSurface(JNIEnv* env, jobject surface)
{
    if (surface)
    {
        if (window)
        {
            ANativeWindow_release(window);
        }
        window = ANativeWindow_fromSurface(env, surface);

        if (window)
        {
            Debug::Log("Surface set");
        }
        else
        {
            Debug::LogError("Surface was not initialized!");
        }
    }
}

void JniBridge::SetAssetManager(JNIEnv* env, jobject assetManagerObj)
{
    assetManager = AAssetManager_fromJava(env, assetManagerObj);

    if (assetManager)
    {
        Debug::Log("Asset manager initialized");
    }
    else
    {
        Debug::LogError("Asset manager was not initialized!");
    }
}

void JniBridge::Initialize()
{
    if (window && assetManager)
    {
        Debug::Log("Initializing");
        Core::Initialize();
    }
    else
    {
        Debug::LogError("Initialization failed due to missing resources");
    }
}

void JniBridge::Tick()
{
    Core::Tick();
}

void JniBridge::UnInitialize()
{
    Core::UnInitialize();

    if (window)
    {
        ANativeWindow_release(window);
        window = nullptr;
    }
}

void JniBridge::Resize(int width, int height)
{
    Screen::SetResolution(width, height);
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
JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativeSetSurface(JNIEnv *env, jobject obj, jobject surface)
{
    JniBridge::SetSurface(env, surface);
}

JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativeSetAssetManager(JNIEnv *env, jobject obj, jobject assetManager)
{
    JniBridge::SetAssetManager(env, assetManager);
}

JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativeInitialize(JNIEnv *env, jobject obj)
{
    JniBridge::Initialize();
}

JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativeTick(JNIEnv* env, jobject obj)
{
    JniBridge::Tick();
}

JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativeUnInitialize(JNIEnv* env, jobject obj)
{
    JniBridge::UnInitialize();
}

JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativeResize(JNIEnv* env, jobject obj, int width, int height)
{
    JniBridge::Resize(width, height);
}
}
