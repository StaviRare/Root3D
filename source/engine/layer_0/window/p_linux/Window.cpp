#include <cstring>
#include <wayland-client.h>

#include "Window.h"
#include "PlatformEventQueue.h"
#include "WaylandHelpers.h"
#include "xdg-shell-client-protocol.h"
#include "xdg-decoration-client-protocol.h"

static uint32 m_width = 0;
static uint32 m_height = 0;
static wl_display* display = nullptr;
static wl_registry* registry = nullptr;
static wl_compositor* compositor = nullptr;
static xdg_wm_base* wmBase = nullptr;
static wl_surface* surface = nullptr;
static xdg_surface* xdgSurface = nullptr;
static xdg_toplevel* toplevel = nullptr;
static zxdg_toplevel_decoration_v1* decoration = nullptr;
static zxdg_decoration_manager_v1* decorationManager = nullptr;
static WindowHandle windowHandles;

static void WaylandEventHandler(WaylandEventType type, void* data);

Window* Window::s_instance = nullptr;

Window* Window::getInstance()
{
    return s_instance;
}

const uint32 Window::GetWidth()
{
    return m_width;
}

const uint32 Window::GetHeight()
{
    return m_height;
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
    m_width = width;
    m_height = height;
}

void Window::Initialize(WindowDesc config)
{
    s_instance = this;
    bool success = true;

    m_width = config.width;
    m_height = config.height;


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


    // set window handle
    windowHandles.display = display;
    windowHandles.surface = surface;


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
    return &windowHandles;
}

void Window::PollEvents()
{
    if (display != nullptr)
    {
        wl_display_dispatch_pending(display);
        wl_display_flush(display);
    }
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
                m_width = width;
                m_height = height;

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