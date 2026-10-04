#include "WaylandHelpers.h"

WaylandHelpers::EventListener WaylandHelpers::eventListener = nullptr;

const wl_registry_listener WaylandHelpers::RegistryListener =
{
    WaylandHelpers::RegistryGlobal,
    WaylandHelpers::RegistryGlobalRemove
};

const xdg_toplevel_listener WaylandHelpers::ToplevelListener =
{
    WaylandHelpers::ToplevelConfigure,
    WaylandHelpers::ToplevelClose
};

const xdg_wm_base_listener WaylandHelpers::WmBaseListener =
{
    WaylandHelpers::WmBasePing
};

const zxdg_toplevel_decoration_v1_listener WaylandHelpers::DecorationListener =
{
    WaylandHelpers::DecorationConfigure
};

const xdg_surface_listener WaylandHelpers::XdgSurfaceListener =
{
    WaylandHelpers::XdgSurfaceConfigure
};

void WaylandHelpers::SetEventListener(EventListener listener)
{
    eventListener = listener;
}

void WaylandHelpers::RegistryGlobal(void*, wl_registry* registry, uint32_t name, const char* interface, uint32_t version)
{
    if (eventListener != nullptr)
    {
        RegistryGlobalData eventData;
        eventData.registry = registry;
        eventData.name = name;
        eventData.interface = interface;
        eventData.version = version;

        eventListener(
            WaylandEventType::RegistryGlobal,
            &eventData);
    }
}

void WaylandHelpers::RegistryGlobalRemove(void*, wl_registry* registry, uint32_t name)
{
    if (eventListener != nullptr)
    {
        RegistryGlobalRemoveData eventData;
        eventData.registry = registry;
        eventData.name = name;

        eventListener(
            WaylandEventType::RegistryGlobalRemove,
            &eventData);
    }
}

void WaylandHelpers::WmBasePing(void*, xdg_wm_base* base, uint32_t serial)
{
    xdg_wm_base_pong(base, serial);
}

void WaylandHelpers::XdgSurfaceConfigure(void*, xdg_surface* surface, uint32_t serial)
{
    if (eventListener != nullptr)
    {
        SurfaceConfigureData eventData;
        eventData.surface = surface;
        eventData.serial = serial;

        eventListener(
            WaylandEventType::SurfaceConfigure,
            &eventData);
    }
}

void WaylandHelpers::DecorationConfigure(void*, zxdg_toplevel_decoration_v1* decoration, uint32_t mode)
{
    if (eventListener != nullptr)
    {
        DecorationConfigureData eventData;
        eventData.decoration = decoration;
        eventData.mode = mode;

        eventListener(
            WaylandEventType::DecorationConfigure,
            &eventData);
    }
}

void WaylandHelpers::ToplevelConfigure(void*, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states)
{
    if (eventListener != nullptr)
    {
        ToplevelConfigureData eventData;
        eventData.toplevel = toplevel;
        eventData.width = width;
        eventData.height = height;
        eventData.states = states;

        eventListener(
            WaylandEventType::ToplevelConfigure,
            &eventData);
    }
}

void WaylandHelpers::ToplevelClose(void*, xdg_toplevel* toplevel)
{
    if (eventListener != nullptr)
    {
        ToplevelCloseData eventData;
        eventData.toplevel = toplevel;

        eventListener(
            WaylandEventType::ToplevelClose,
            &eventData);
    }
}