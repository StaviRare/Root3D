#include <cstring>
#include <wayland-egl.h>
#include <wayland-client.h>
#include <EGL/egl.h>

#include "Log.h"
#include "Window.h"
#include "PlatformEventQueue.h"
#include "xdg-shell-client-protocol.h"
#include "xdg-decoration-client-protocol.h"
#include "OpenGLES2.h"
#include "WaylandHelpers.h"

static EGLHandles eglHandles;
static EGLDisplay eglDisplay = EGL_NO_DISPLAY;
static EGLSurface eglSurface = EGL_NO_SURFACE;
static EGLContext eglContext = EGL_NO_CONTEXT;
static EGLConfig eglConfig = nullptr;

static bool running = true;
static bool configured = false;
static wl_display* display = nullptr;
static wl_registry* registry = nullptr;
static wl_compositor* compositor = nullptr;
static xdg_wm_base* wmBase = nullptr;
static zxdg_decoration_manager_v1* decorationManager = nullptr;
static wl_surface* surface = nullptr;
static xdg_surface* xdgSurface = nullptr;
static xdg_toplevel* toplevel = nullptr;
static zxdg_toplevel_decoration_v1* decoration = nullptr;
static wl_egl_window* eglWindow = nullptr;
static uint32 windowWidth = 800;
static uint32 windowHeight = 600;

// Forward declarations
static void WaylandEventHandler(WaylandEventType type, void* data);
static bool CreateEGLSurfaceAndMakeCurrent();
static void DestroyEGLSurfaceAndUnbindContext();


static void registryGlobal(void*, wl_registry* registry, uint32_t name, const char* interface, uint32_t)
{
    if (std::strcmp(interface, wl_compositor_interface.name) == 0)
    {
        compositor =static_cast<wl_compositor*>(
                wl_registry_bind(registry, name, &wl_compositor_interface, 4)
            );
    }
    else if (std::strcmp(interface, xdg_wm_base_interface.name) == 0)
    {
        wmBase = static_cast<xdg_wm_base*>(
            wl_registry_bind(registry, name, &xdg_wm_base_interface, 1)
        );

        xdg_wm_base_add_listener(wmBase, &WaylandHelpers::WmBaseListener, nullptr);
    }
    else if (std::strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0)
    {
        decorationManager =
            static_cast<zxdg_decoration_manager_v1*>(
                wl_registry_bind(
                    registry,
                    name,
                    &zxdg_decoration_manager_v1_interface,
                    1
                    )
                );
    }
}

static void registryGlobalRemove(void*, wl_registry*, uint32_t)
{
    // Nothing for now.
}


static const wl_registry_listener registryListener =
{
    registryGlobal,
    registryGlobalRemove
};


Window* Window::s_instance = nullptr;


Window* Window::getInstance()
{
    Window* returnValue = s_instance;
    return returnValue;
}

const uint32 Window::GetWidth()
{
    const uint32 returnValue = windowWidth;
    return returnValue;
}

const uint32 Window::GetHeight()
{
    const uint32 returnValue = windowHeight;
    return returnValue;
}

void Window::SetFullScreen(bool enable)
{
    if (toplevel != nullptr)
    {
        if (enable)
        {
            xdg_toplevel_set_fullscreen(toplevel, nullptr);
        }
        else
        {
            xdg_toplevel_unset_fullscreen(toplevel);
        }
    }
}

void Window::SetResolution(uint32 width,uint32 height)
{
    windowWidth = width;
    windowHeight = height;

    if (eglWindow != nullptr)
    {
        wl_egl_window_resize(
            eglWindow,
            static_cast<int32_t>(width),
            static_cast<int32_t>(height),
            0,
            0
            );
    }
}

void Window::Initialize(WindowDesc config)
{
    bool success = true;

    s_instance = this;
    running = true;
    configured = false;

    WaylandHelpers::SetEventListener(WaylandEventHandler);

    display = wl_display_connect(nullptr);

    if (display == nullptr)
    {
        success = false;
    }

    if (success)
    {
        registry = wl_display_get_registry(display);

        if (registry == nullptr)
        {
            success = false;
        }
    }

    if (success)
    {
        wl_registry_add_listener(registry, &registryListener, nullptr);

        if (wl_display_roundtrip(display) < 0)
        {
            success = false;
        }
    }

    if (success)
    {
        if (compositor == nullptr ||
            wmBase == nullptr ||
            decorationManager == nullptr)
        {
            success = false;
        }
    }

    if (success)
    {
        surface = wl_compositor_create_surface(compositor);

        if (surface == nullptr)
        {
            success = false;
        }
    }

    if (success)
    {
        xdgSurface = xdg_wm_base_get_xdg_surface(wmBase, surface);

        if (xdgSurface == nullptr)
        {
            success = false;
        }
    }

    if (success)
    {
        xdg_surface_add_listener(
            xdgSurface,
            &WaylandHelpers::xdgSurfaceListener,
            nullptr
            );

        toplevel = xdg_surface_get_toplevel(xdgSurface);

        if (toplevel == nullptr)
        {
            success = false;
        }
    }

    if (success)
    {
        xdg_toplevel_add_listener(
            toplevel,
            &WaylandHelpers::ToplevelListener,
            nullptr
            );

        xdg_toplevel_set_title(
            toplevel,
            config.title
            );

        xdg_toplevel_set_app_id(
            toplevel,
            "root3d.com"
            );
    }

    if (success)
    {
        decoration =
            zxdg_decoration_manager_v1_get_toplevel_decoration(
                decorationManager,
                toplevel
                );

        if (decoration == nullptr)
        {
            success = false;
        }
    }

    if (success)
    {
        zxdg_toplevel_decoration_v1_add_listener(
            decoration,
            &WaylandHelpers::decorationListener,
            nullptr
            );

        zxdg_toplevel_decoration_v1_set_mode(
            decoration,
            ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE
            );

        wl_surface_commit(surface);

        while (!configured && running)
        {
            if (wl_display_dispatch(display) < 0)
            {
                running = false;
            }
        }

        if (!running)
        {
            success = false;
        }
    }

    if (success)
    {
        eglWindow =
            wl_egl_window_create(
                surface,
                static_cast<int32_t>(windowWidth),
                static_cast<int32_t>(windowHeight)
                );

        if (eglWindow == nullptr)
        {
            success = false;
        }
    }

    if (success)
    {
        success = CreateEGLSurfaceAndMakeCurrent();
    }

    if (success)
    {
        if (config.fullscreen)
        {
            SetFullScreen(true);
        }
    }
}

void Window::Pause()
{
    // No need
}

void Window::Resume()
{
    // No need
}

void Window::UnInitialize()
{
    DestroyEGLSurfaceAndUnbindContext();

    if (eglWindow != nullptr)
    {
        wl_egl_window_destroy(eglWindow);
        eglWindow = nullptr;
    }

    if (decoration != nullptr)
    {
        zxdg_toplevel_decoration_v1_destroy(decoration);
        decoration = nullptr;
    }

    if (toplevel != nullptr)
    {
        xdg_toplevel_destroy(toplevel);
        toplevel = nullptr;
    }

    if (xdgSurface != nullptr)
    {
        xdg_surface_destroy(xdgSurface);
        xdgSurface = nullptr;
    }

    if (surface != nullptr)
    {
        wl_surface_destroy(surface);
        surface = nullptr;
    }

    if (wmBase != nullptr)
    {
        xdg_wm_base_destroy(wmBase);
        wmBase = nullptr;
    }

    if (decorationManager != nullptr)
    {
        zxdg_decoration_manager_v1_destroy(decorationManager);
        decorationManager = nullptr;
    }

    if (compositor != nullptr)
    {
        wl_compositor_destroy(compositor);
        compositor = nullptr;
    }

    if (registry != nullptr)
    {
        wl_registry_destroy(registry);
        registry = nullptr;
    }

    if (display != nullptr)
    {
        wl_display_disconnect(display);
        display = nullptr;
    }

    configured = false;
    running = false;

    if (s_instance == this)
    {
        s_instance = nullptr;
    }
}

void* Window::GetNativeHandle()
{
    eglHandles.display = eglDisplay;
    eglHandles.surface = eglSurface;
    return &eglHandles;
}

void Window::PollEvents()
{
    if (display != nullptr)
    {
        // pending - process without blocking.
        if (wl_display_dispatch_pending(display) < 0)
        {
            running = false;
        }
    }
}


static bool CreateEGLSurfaceAndMakeCurrent()
{
    bool returnValue = true;

    eglDisplay = eglGetDisplay(static_cast<EGLNativeDisplayType>(display));

    if (eglDisplay == EGL_NO_DISPLAY)
    {
        returnValue = false;
    }

    if (returnValue)
    {
        if (!eglInitialize(
                eglDisplay,
                nullptr,
                nullptr))
        {
            returnValue = false;
        }
    }

    if (returnValue)
    {
        if (!eglBindAPI(EGL_OPENGL_ES_API))
        {
            returnValue = false;
        }
    }

    if (returnValue)
    {
        const EGLint configAttributes[] =
        {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RED_SIZE,        8,
            EGL_GREEN_SIZE,      8,
            EGL_BLUE_SIZE,       8,
            EGL_ALPHA_SIZE,      8,
            EGL_DEPTH_SIZE,      16,
            EGL_NONE
        };

        EGLint configCount = 0;

        if (!eglChooseConfig(
                eglDisplay,
                configAttributes,
                &eglConfig,
                1,
                &configCount))
        {
            returnValue = false;
        }
        else if (configCount == 0)
        {
            returnValue = false;
        }
    }

    if (returnValue)
    {
        eglSurface =
            eglCreateWindowSurface(
                eglDisplay,
                eglConfig,
                reinterpret_cast<EGLNativeWindowType>(
                    eglWindow
                    ),
                nullptr
                );

        if (eglSurface == EGL_NO_SURFACE)
        {
            returnValue = false;
        }
    }

    if (returnValue)
    {
        const EGLint contextAttributes[] =
            {
                EGL_CONTEXT_CLIENT_VERSION,
                2,
                EGL_NONE
            };

        eglContext =
            eglCreateContext(
                eglDisplay,
                eglConfig,
                EGL_NO_CONTEXT,
                contextAttributes
                );

        if (eglContext == EGL_NO_CONTEXT)
        {
            returnValue = false;
        }
    }

    if (returnValue)
    {
        if (!eglMakeCurrent(
                eglDisplay,
                eglSurface,
                eglSurface,
                eglContext))
        {
            returnValue = false;
        }
    }

    return returnValue;
}

static void DestroyEGLSurfaceAndUnbindContext()
{
    if (eglDisplay != EGL_NO_DISPLAY)
    {
        if (eglContext != EGL_NO_CONTEXT)
        {
            eglMakeCurrent(
                eglDisplay,
                EGL_NO_SURFACE,
                EGL_NO_SURFACE,
                EGL_NO_CONTEXT
                );

            eglDestroyContext(
                eglDisplay,
                eglContext
                );

            eglContext = EGL_NO_CONTEXT;
        }

        if (eglSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(
                eglDisplay,
                eglSurface
                );

            eglSurface = EGL_NO_SURFACE;
        }

        eglTerminate(eglDisplay);

        eglDisplay = EGL_NO_DISPLAY;
    }

    eglConfig = nullptr;
}


static void WaylandEventHandler(WaylandEventType type, void* data)
{
    switch (type)
    {
        case WaylandEventType::ToplevelConfigure:
        {
            ToplevelConfigureData* configureData =
                static_cast<ToplevelConfigureData*>(data);

            int32_t width = configureData->width;
            int32_t height = configureData->height;

            if (width > 0)
            {
                windowWidth = static_cast<uint32>(width);
            }

            if (height > 0)
            {
                windowHeight = static_cast<uint32>(height);
            }

            if (eglWindow != nullptr &&
                width > 0 &&
                height > 0)
            {
                wl_egl_window_resize(
                    eglWindow,
                    width,
                    height,
                    0,
                    0
                    );
            }

            if (width > 0 && height > 0)
            {
                PlatformEvent ev;
                ev.type = EventType::Resize;
                ev.width = static_cast<uint32>(width);
                ev.height = static_cast<uint32>(height);

                PlatformEventQueue::Push(ev);
            }

            break;
        }

        case WaylandEventType::ToplevelClose:
        {
            PlatformEvent ev;
            ev.type = EventType::Close;
            PlatformEventQueue::Push(ev);
            break;
        }

        case WaylandEventType::SurfaceConfigure:
        {
            configured = true;
            break;
        }
    }
}