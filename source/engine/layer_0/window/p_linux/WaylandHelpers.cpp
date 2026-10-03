#include "WaylandHelpers.h"

WaylandHelpers::EventListener WaylandHelpers::eventListener = nullptr;

const xdg_toplevel_listener WaylandHelpers::ToplevelListener =
{
    WaylandHelpers::ToplevelConfigure,
    WaylandHelpers::ToplevelClose
};

const xdg_wm_base_listener WaylandHelpers::WmBaseListener =
{
    WaylandHelpers::WmBasePing
};

const zxdg_toplevel_decoration_v1_listener WaylandHelpers::decorationListener =
{
    WaylandHelpers::decorationConfigure
};


void WaylandHelpers::WmBasePing(void*, xdg_wm_base* base,uint32_t serial)
{
    xdg_wm_base_pong(base, serial);
}

void WaylandHelpers::SetEventListener(EventListener listener)
{
    eventListener = listener;
}


void WaylandHelpers::decorationConfigure(void*, zxdg_toplevel_decoration_v1*, uint32_t)
{
    // Nothing for now.
}

const xdg_surface_listener WaylandHelpers::xdgSurfaceListener =
{
    WaylandHelpers::xdgSurfaceConfigure
};

void WaylandHelpers::xdgSurfaceConfigure(void*, xdg_surface* surface, uint32_t serial)
{
    xdg_surface_ack_configure(surface, serial);

    if (eventListener != nullptr)
    {
        eventListener(
            WaylandEventType::SurfaceConfigure,
            nullptr
            );
    }
}

void WaylandHelpers::ToplevelConfigure(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states)
{
    if (eventListener != nullptr)
    {
        ToplevelConfigureData eventData;
        eventData.width = width;
        eventData.height = height;

        eventListener(
            WaylandEventType::ToplevelConfigure,
            &eventData
            );
    }
}

void WaylandHelpers::ToplevelClose(void* data, xdg_toplevel* toplevel)
{
    if (eventListener != nullptr)
    {
        eventListener(
            WaylandEventType::ToplevelClose,
            nullptr
            );
    }
}