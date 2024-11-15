#include <EGL/egl.h>
#include <android/native_window.h>
#include <vector>

#include "Log.h"
#include "Screen.h"
#include "JniBridge.h"
#include "Config.h"
#include "OpenGLES1.h"

static int width;
static int height;
static std::vector<Func2Arg<int, int>> callbacks;

static EGLHandles eglHandles;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;

bool CreateEGLSurfaceAndMakeCurrent();
void DestroyEGLSurfaceAndUnbindContext();

const int Screen::GetWidth()
{
    return width;
}

const int Screen::GetHeight()
{
    return height;
}

void Screen::SetFullScreen(bool enable)
{
    // Implement
}

void Screen::SetResolution(int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;

    for (auto& callback : callbacks)
    {
        callback(width, height);
    }
}

void Screen::RegisterResizeCallback(Func2Arg<int, int> ptr)
{
    if (ptr && std::find(callbacks.begin(), callbacks.end(), ptr) == callbacks.end())
    {
        callbacks.push_back(ptr);
    }
}

void Screen::UnRegisterResizeCallback(Func2Arg<int, int> ptr)
{
    callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), ptr), callbacks.end());
}

void Screen::Initialize()
{
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY)
    {
        Log::Error("Failed to get EGL display");
        return;
    }

    if (!eglInitialize(display, nullptr, nullptr))
    {
        Log::Error("Failed to initialize EGL");
        return;
    }

    if (!CreateEGLSurfaceAndMakeCurrent())
    {
        Log::Error("Failed to initialize EGL surface and context");
        return;
    }
}

void Screen::UnInitialize()
{
    DestroyEGLSurfaceAndUnbindContext();

    if (context != EGL_NO_CONTEXT)
    {
        eglDestroyContext(display, context);
        context = EGL_NO_CONTEXT;
    }

    if (display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
        display = EGL_NO_DISPLAY;
    }
}

void Screen::Resume()
{
    if (display != EGL_NO_DISPLAY && surface == EGL_NO_SURFACE)
    {
        if (!CreateEGLSurfaceAndMakeCurrent())
        {
            Log::Error("Failed to resume EGL surface and context");
            return;
        }
    }
}

void Screen::Pause()
{
    DestroyEGLSurfaceAndUnbindContext();
}

void Screen::PollEvents()
{
    // Implement event polling if necessary
}

void* Screen::GetNativeHandle()
{
    eglHandles.display = display;
    eglHandles.surface = surface;
    return &eglHandles;
}

bool CreateEGLSurfaceAndMakeCurrent()
{
    ANativeWindow* window = JniBridge::GetNativeWindow();
    if (!window)
    {
        Log::Error("Failed to get native window");
        return false;
    }

    EGLConfig config;
    EGLint numConfigs;
    EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RED_SIZE,        8,
            EGL_GREEN_SIZE,      8,
            EGL_BLUE_SIZE,       8,
            EGL_ALPHA_SIZE,      8,
            EGL_NONE
    };

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs))
    {
        Log::Error("Failed to choose EGL config");
        return false;
    }

    surface = eglCreateWindowSurface(display, config, window, nullptr);
    if (surface == EGL_NO_SURFACE)
    {
        Log::Error("Failed to create EGL window surface");
        return false;
    }

    // Create the EGL context if it doesn't exist
    if (context == EGL_NO_CONTEXT)
    {
        EGLint contextAttribs[] = {
                EGL_CONTEXT_CLIENT_VERSION, 1,
                EGL_NONE
        };
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
        if (context == EGL_NO_CONTEXT)
        {
            Log::Error("Failed to create EGL context");
            return false;
        }
    }

    if (!eglMakeCurrent(display, surface, surface, context))
    {
        Log::Error("Failed to make EGL context current");
        return false;
    }

    // Query the surface dimensions
    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);

    // Notify registered callbacks about the new dimensions
    for (auto& callback : callbacks)
    {
        callback(width, height);
    }

    return true;
}

void DestroyEGLSurfaceAndUnbindContext()
{
    if (display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    if (surface != EGL_NO_SURFACE)
    {
        eglDestroySurface(display, surface);
        surface = EGL_NO_SURFACE;
    }
}