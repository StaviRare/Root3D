#include <EGL/egl.h>
#include <android/native_window.h>
#include <vector>

#include "Log.h"
#include "Window.h"
#include "JniBridge.h"
#include "OpenGLES1.h"

static EGLHandles eglHandles;
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;
Window* Window::s_instance = nullptr;

bool CreateEGLSurfaceAndMakeCurrent();
void DestroyEGLSurfaceAndUnbindContext();

Window* Window::getInstance()
{
    return s_instance;
}

const uint32 Window::GetWidth()
{
    uint32 returnValue = 0;
    ANativeWindow* window = JniBridge::GetNativeWindow();
    if (window)
    {
        returnValue = static_cast<uint32>(ANativeWindow_getWidth(window));
    }

    return returnValue;
}

const uint32 Window::GetHeight()
{
    uint32 returnValue = 0;
    ANativeWindow* window = JniBridge::GetNativeWindow();
    if (window)
    {
        returnValue = static_cast<uint32>(ANativeWindow_getHeight(window));
    }

    return returnValue;
}

void Window::SetFullScreen(bool enable)
{
    // No need here.
}

void Window::SetResolution(uint32 width, uint32 height)
{
    // No need here.
}

void Window::Initialize(WindowDesc desc)
{
    if (s_instance != nullptr)
    {
        ENGINE_ERROR("Window already initialized.");
    }
    else
    {
        s_instance = this;

        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            Log::Error("Failed to get EGL display");
            return;
        }

        if (!eglInitialize(display, nullptr, nullptr)) {
            Log::Error("Failed to initialize EGL");
            return;
        }

        if (!CreateEGLSurfaceAndMakeCurrent()) {
            Log::Error("Failed to initialize EGL surface and context");
            return;
        }
    }
}

void Window::UnInitialize()
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

void Window::Resume()
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

void Window::Pause()
{
    DestroyEGLSurfaceAndUnbindContext();
}

void Window::PollEvents()
{
    // No need here.
}

void* Window::GetNativeHandle()
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
            EGL_DEPTH_SIZE,      16,
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

    EGLint w = 0;
    EGLint h = 0;

    // Query the surface dimensions
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    Window* wnd = Window::getInstance();
    if (wnd) {
        wnd->SetResolution(
                static_cast<uint32>(w),
                static_cast<uint32>(h)
        );
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