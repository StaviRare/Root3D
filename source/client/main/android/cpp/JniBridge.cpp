#include "JniBridge.h"
#include "Log.h"
#include "Runtime.h"
#include "PlatformEventQueue.h"

Runtime* JniBridge::runtime = nullptr;
ANativeWindow* JniBridge::window = nullptr;
AAssetManager* JniBridge::assetManager = nullptr;

void JniBridge::OnSurfaceCreated(JNIEnv* env, jobject surface)
{
    ENGINE_INFO("On Surface Created");

    if (surface)
    {
        if (window)
        {
            ANativeWindow_release(window);
        }

        window = ANativeWindow_fromSurface(env, surface);

        if (window)
        {
            PlatformEvent ev;
            ev.type = EventType::SurfaceCreated;
            PlatformEventQueue::Push(ev);
        }
        else
        {
            ENGINE_ERROR("Surface was not initialized!");
        }
    }
}

void JniBridge::OnSurfaceDestroyed()
{
    ENGINE_INFO("On Surface Destroyed");

    if (window)
    {
        PlatformEvent ev;
        ev.type = EventType::SurfaceDestroyed;
        PlatformEventQueue::Push(ev);

        ANativeWindow_release(window);
        window = nullptr;
    }
}

void JniBridge::SetAssetManager(JNIEnv* env, jobject assetManagerObj)
{
    assetManager = AAssetManager_fromJava(env, assetManagerObj);

    if (assetManager)
    {
        ENGINE_INFO("Asset manager initialized");
    }
    else
    {
        ENGINE_ERROR("Asset manager was not initialized!");
    }
}

void JniBridge::Initialize()
{
    if (window && assetManager)
    {
        runtime = new Runtime();
        runtime->Initialize();
    }
    else
    {
        ENGINE_ERROR("Initialization failed due to missing resources");
    }
}

void JniBridge::UnInitialize()
{
    if(runtime)
    {
        runtime->UnInitialize();
        delete runtime;
        runtime = nullptr;
    }

    if (window)
    {
        ANativeWindow_release(window);
        window = nullptr;
    }
}

void JniBridge::Pause()
{
    if(runtime)
    {
        runtime->Pause();
    }
}

void JniBridge::Resume()
{
    if(runtime)
    {
        runtime->Resume();
    }
}

void JniBridge::FocusChanged(bool hasFocus)
{
    PlatformEvent ev;
    ev.type = hasFocus
            ? EventType::FocusGained
            : EventType::FocusLost;
    PlatformEventQueue::Push(ev);
}

void JniBridge::Tick()
{
    if(runtime)
    {
        runtime->Tick();
    }
}

void JniBridge::Resize(int width, int height)
{
    PlatformEvent ev;
    ev.type = EventType::Resize;
    ev.width = width;
    ev.height = height;
    PlatformEventQueue::Push(ev);
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
    JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativeOnSurfaceCreated(JNIEnv *env, jobject obj, jobject surface)
    {
        JniBridge::OnSurfaceCreated(env, surface);
    }

    JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativeOnSurfaceDestroyed()
    {
        JniBridge::OnSurfaceDestroyed();
    }

    JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativeSetAssetManager(JNIEnv *env, jobject obj, jobject assetManager)
    {
        JniBridge::SetAssetManager(env, assetManager);
    }

    JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativeInitialize(JNIEnv *env, jobject obj)
    {
        JniBridge::Initialize();
    }

    JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativeResume(JNIEnv* env, jobject obj)
    {
        JniBridge::Resume();
    }

    JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativePause(JNIEnv* env, jobject obj)
    {
        JniBridge::Pause();
    }

    JNIEXPORT void JNICALL Java_com_root3d_player_EnginePlayer_nativeFocusChanged(JNIEnv* env, jobject obj, jboolean hasFocus)
    {
        JniBridge::FocusChanged(hasFocus);
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
