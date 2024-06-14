#include <EGL/egl.h>
#include <android/native_window.h>
#include <vector>

#include "Debug.h"
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

    for (const auto &callback: callbacks)
    {
        if (callback)
        {
            callback(width, height);
        }
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
    ANativeWindow* window = JniBridge::GetNativeWindow();

    const EGLint attribs[] =
    {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    EGLint format;

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (display == EGL_NO_DISPLAY)
    {
        Debug::LogError("Failed to get default display.");
        return;
    }

    if (!eglInitialize(display, nullptr, nullptr))
    {
        Debug::LogError("Failed to initialize EGL.");
        return;
    }

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs))
    {
        Debug::LogError("Failed to choose config.");
        return;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
    {
        Debug::LogError("Failed to get config attrib.");
        return;
    }

    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, window, nullptr);

    if (surface == EGL_NO_SURFACE)
    {
        Debug::LogError("Failed to create window surface.");
        return;
    }

    context = eglCreateContext(display, config, EGL_NO_CONTEXT, nullptr);

    if (context == EGL_NO_CONTEXT)
    {
        Debug::LogError("Failed to create EGL context.");
        return;
    }

    if (!eglMakeCurrent(display, surface, surface, context))
    {
        Debug::LogError("Failed to make context current.");
        return;
    }


    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);
}

void Screen::UnInitialize()
{
    if (display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(display, context);
        }

        if (surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(display, surface);
        }

        eglTerminate(display);
    }

    display = EGL_NO_DISPLAY;
    surface = EGL_NO_SURFACE;
    context = EGL_NO_CONTEXT;
}

void Screen::PollEvents()
{
    // Implement
}

void* Screen::GetNativeHandle() {
    eglHandles.display = display;
    eglHandles.surface = surface;
    return &eglHandles;
}