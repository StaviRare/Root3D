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

bool CreateEGLSurface();

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

    // Recreate the EGL surface with the new resolution
    if (!CreateEGLSurface())
    {
        ENGINE_ERROR("Failed to recreate EGL surface.");
    }

    for (const auto &callback : callbacks)
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
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (display == EGL_NO_DISPLAY)
    {
        ENGINE_ERROR("Failed to get default display.");
        return;
    }

    if (!eglInitialize(display, nullptr, nullptr))
    {
        ENGINE_ERROR("Failed to initialize EGL.");
        return;
    }

    context = eglCreateContext(display, nullptr, EGL_NO_CONTEXT, nullptr);

    if (context == EGL_NO_CONTEXT)
    {
        ENGINE_ERROR("Failed to create EGL context.");
        return;
    }

    if (!CreateEGLSurface())
    {
        ENGINE_ERROR("Failed to create initial EGL surface.");
    }
}

void Screen::UnInitialize()
{
    if (display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(display, context);
            context = EGL_NO_CONTEXT;
        }

        if (surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(display, surface);
            surface = EGL_NO_SURFACE;
        }

        eglTerminate(display);
        display = EGL_NO_DISPLAY;
    }
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

bool CreateEGLSurface()
{
    if (surface != EGL_NO_SURFACE)
    {
        eglDestroySurface(display, surface);
    }

    ANativeWindow* window = JniBridge::GetNativeWindow();
    if (window == nullptr)
    {
        ENGINE_ERROR("Failed to get native window.");
        return false;
    }

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

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs))
    {
        ENGINE_ERROR("Failed to choose config.");
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
    {
        ENGINE_ERROR("Failed to get config attrib.");
        return false;
    }

    ANativeWindow_setBuffersGeometry(window, width, height, format);

    surface = eglCreateWindowSurface(display, config, window, nullptr);

    if (surface == EGL_NO_SURFACE)
    {
        ENGINE_ERROR("Failed to create window surface.");
        return false;
    }

    if (!eglMakeCurrent(display, surface, surface, context))
    {
        ENGINE_ERROR("Failed to make context current.");
        return false;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);

    return true;
}