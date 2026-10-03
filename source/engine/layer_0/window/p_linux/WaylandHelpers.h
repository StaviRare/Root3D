#pragma once

#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"
#include "xdg-decoration-client-protocol.h"

enum class WaylandEventType
{
    ToplevelConfigure,
    ToplevelClose,
    SurfaceConfigure
};

struct WaylandEvent
{
    WaylandEventType type;
    void* data;
};

struct ToplevelConfigureData
{
    int32_t width;
    int32_t height;
};

class WaylandHelpers
{
    public:
    using EventListener = void (*)(WaylandEventType type, void* data);
    static void SetEventListener(EventListener listener);
    static const xdg_toplevel_listener ToplevelListener;
    static const xdg_wm_base_listener WmBaseListener;
    static const zxdg_toplevel_decoration_v1_listener decorationListener;
    static const xdg_surface_listener xdgSurfaceListener;

    private:
    static void xdgSurfaceConfigure(void*, xdg_surface* surface, uint32_t serial);
    static void decorationConfigure(void*, zxdg_toplevel_decoration_v1*, uint32_t);
    static void WmBasePing(void*, xdg_wm_base* base,uint32_t serial);
    static void ToplevelConfigure(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states);
    static void ToplevelClose(void* data, xdg_toplevel* toplevel);
    static EventListener eventListener;
};