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

static wl_display* display = nullptr;
static wl_registry* registry = nullptr;
static wl_compositor* compositor = nullptr;
static xdg_wm_base* wmBase = nullptr;
static wl_surface* surface = nullptr;
static xdg_surface* xdgSurface = nullptr;
static xdg_toplevel* toplevel = nullptr;
static wl_egl_window* eglWindow = nullptr;
static zxdg_toplevel_decoration_v1* decoration = nullptr;
static zxdg_decoration_manager_v1* decorationManager = nullptr;

static bool CreateEGLSurfaceAndMakeCurrent();
static void DestroyEGLSurfaceAndUnbindContext();
static void WaylandEventHandler(WaylandEventType type, void* data);

Window* Window::s_instance = nullptr;

Window* Window::getInstance()
{
    return s_instance;
}

const uint32 Window::GetWidth()
{
    uint32 returnValue = 0;

    if (surface != nullptr && eglWindow != nullptr)
    {
        int width = 0;

        wl_egl_window_get_attached_size(
            eglWindow,
            &width,
            nullptr);

        returnValue = static_cast<uint32>(width);
    }

    return returnValue;
}

const uint32 Window::GetHeight()
{
    uint32 returnValue = 0;

    if (surface != nullptr && eglWindow != nullptr)
    {
        int height = 0;

        wl_egl_window_get_attached_size(
            eglWindow,
            nullptr,
            &height);

        returnValue = static_cast<uint32>(height);
    }

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

void Window::SetResolution(uint32 width, uint32 height)
{
    if (eglWindow != nullptr)
    {
        wl_egl_window_resize(
            eglWindow,
            static_cast<int32_t>(width),
            static_cast<int32_t>(height),
            0,
            0);
    }
}

void Window::Initialize(WindowDesc config)
{
    s_instance = this;
    bool success = true;

    WaylandHelpers::SetEventListener(WaylandEventHandler);

    display = wl_display_connect(nullptr);
    success = display != nullptr;

    if (success)
    {
        registry = wl_display_get_registry(display);
        success = registry != nullptr;
    }

    if (success)
    {
        wl_registry_add_listener(
            registry,
            &WaylandHelpers::RegistryListener,
            nullptr);

        success = wl_display_roundtrip(display) >= 0;
    }

    if (success)
    {
        success =
            compositor != nullptr &&
            wmBase != nullptr &&
            decorationManager != nullptr;
    }

    if (success)
    {
        surface = wl_compositor_create_surface(compositor);
        success = surface != nullptr;
    }

    if (success)
    {
        xdgSurface = xdg_wm_base_get_xdg_surface(
            wmBase,
            surface);

        success = xdgSurface != nullptr;
    }

    if (success)
    {
        xdg_surface_add_listener(
            xdgSurface,
            &WaylandHelpers::XdgSurfaceListener,
            nullptr);

        toplevel = xdg_surface_get_toplevel(xdgSurface);
        success = toplevel != nullptr;
    }

    if (success)
    {
        xdg_toplevel_add_listener(
            toplevel,
            &WaylandHelpers::ToplevelListener,
            nullptr);

        xdg_toplevel_set_title(
            toplevel,
            config.title);

        xdg_toplevel_set_app_id(
            toplevel,
            "com.root3d.game");
    }

    if (success)
    {
        decoration =
            zxdg_decoration_manager_v1_get_toplevel_decoration(
                decorationManager,
                toplevel);

        success = decoration != nullptr;
    }

    if (success)
    {
        zxdg_toplevel_decoration_v1_add_listener(
            decoration,
            &WaylandHelpers::DecorationListener,
            nullptr);

        zxdg_toplevel_decoration_v1_set_mode(
            decoration,
            ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);

        wl_surface_commit(surface);

        success = wl_display_roundtrip(display) >= 0;
    }

    if (success)
    {
        eglWindow =
            wl_egl_window_create(
                surface,
                static_cast<int32_t>(config.width),
                static_cast<int32_t>(config.height));

        success = eglWindow != nullptr;
    }

    if (success)
    {
        success = CreateEGLSurfaceAndMakeCurrent();
    }

    if (success && config.fullscreen)
    {
        SetFullScreen(true);
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
        wl_display_dispatch_pending(display);
        wl_display_flush(display);
    }
}

static bool CreateEGLSurfaceAndMakeCurrent()
{
    bool success = true;

    eglDisplay
        = eglGetDisplay(static_cast<EGLNativeDisplayType>(display));

    if (eglDisplay == EGL_NO_DISPLAY)
    {
        success = false;
    }

    if (success)
    {
        bool initializationFailed
            = !eglInitialize(eglDisplay, nullptr, nullptr);

        if (initializationFailed)
        {
            success = false;
        }
    }

    if (success)
    {
        bool apiBindingFailed
            = !eglBindAPI(EGL_OPENGL_ES_API);

        if (apiBindingFailed)
        {
            success = false;
        }
    }

    if (success)
    {
        const EGLint configAttributes[] =
        {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
        };

        EGLint configCount = 0;
        bool configSelectionFailed =
            !eglChooseConfig(eglDisplay, configAttributes, &eglConfig, 1, &configCount)
            || configCount == 0;

        if (configSelectionFailed)
        {
            success = false;
        }
    }

    if (success)
    {
        eglSurface = eglCreateWindowSurface(
            eglDisplay,
            eglConfig,
            reinterpret_cast<EGLNativeWindowType>(eglWindow),
            nullptr);

        if (eglSurface == EGL_NO_SURFACE)
        {
            success = false;
        }
    }

    if (success)
    {
        const EGLint contextAttributes[] =
        {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

        eglContext = eglCreateContext(
            eglDisplay,
            eglConfig,
            EGL_NO_CONTEXT,
            contextAttributes);

        if (eglContext == EGL_NO_CONTEXT)
        {
            success = false;
        }
    }

    if (success)
    {
        bool makeCurrentFailed =
            !eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

        if (makeCurrentFailed)
        {
            success = false;
        }
    }

    return success;
}

static void DestroyEGLSurfaceAndUnbindContext()
{
    if (eglDisplay != EGL_NO_DISPLAY)
    {
        if (eglContext != EGL_NO_CONTEXT)
        {
            eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroyContext(eglDisplay, eglContext);
            eglContext = EGL_NO_CONTEXT;
        }

        if (eglSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(eglDisplay, eglSurface);
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
        case WaylandEventType::RegistryGlobal:
        {
            RegistryGlobalData* eventData =
                static_cast<RegistryGlobalData*>(data);

            if (std::strcmp(
                    eventData->interface,
                    wl_compositor_interface.name) == 0)
            {
                compositor =
                    static_cast<wl_compositor*>(
                        wl_registry_bind(
                            eventData->registry,
                            eventData->name,
                            &wl_compositor_interface,
                            4));
            }
            else if (std::strcmp(
                         eventData->interface,
                         xdg_wm_base_interface.name) == 0)
            {
                wmBase =
                    static_cast<xdg_wm_base*>(
                        wl_registry_bind(
                            eventData->registry,
                            eventData->name,
                            &xdg_wm_base_interface,
                            1));

                xdg_wm_base_add_listener(
                    wmBase,
                    &WaylandHelpers::WmBaseListener,
                    nullptr);
            }
            else if (std::strcmp(
                         eventData->interface,
                         zxdg_decoration_manager_v1_interface.name) == 0)
            {
                decorationManager =
                    static_cast<zxdg_decoration_manager_v1*>(
                        wl_registry_bind(
                            eventData->registry,
                            eventData->name,
                            &zxdg_decoration_manager_v1_interface,
                            1));
            }

            break;
        }

        case WaylandEventType::RegistryGlobalRemove:
        {
            break;
        }

        case WaylandEventType::ToplevelConfigure:
        {
            ToplevelConfigureData* configureData =
                static_cast<ToplevelConfigureData*>(data);

            int32_t width = configureData->width;
            int32_t height = configureData->height;
            bool validSize = width > 0 && height > 0;

            if(validSize){
                if (eglWindow != nullptr)
                {
                    wl_egl_window_resize(
                        eglWindow,
                        width,
                        height,
                        0,
                        0);
                }

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
            SurfaceConfigureData* configureData =
                static_cast<SurfaceConfigureData*>(data);

            xdg_surface_ack_configure(
                configureData->surface,
                configureData->serial);

            break;
        }

        case WaylandEventType::DecorationConfigure:
        {
            break;
        }
    }
}