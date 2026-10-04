#pragma once

#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"
#include "xdg-decoration-client-protocol.h"

enum class WaylandEventType
{
    RegistryGlobal,
    RegistryGlobalRemove,
    ToplevelConfigure,
    ToplevelClose,
    SurfaceConfigure,
    DecorationConfigure
};

struct WaylandEvent
{
    WaylandEventType type;
    void* data;
};

struct RegistryGlobalData
{
    wl_registry* registry;
    uint32_t name;
    const char* interface;
    uint32_t version;
};

struct RegistryGlobalRemoveData
{
    wl_registry* registry;
    uint32_t name;
};

struct ToplevelConfigureData
{
    xdg_toplevel* toplevel;
    int32_t width;
    int32_t height;
    wl_array* states;
};

struct ToplevelCloseData
{
    xdg_toplevel* toplevel;
};

struct SurfaceConfigureData
{
    xdg_surface* surface;
    uint32_t serial;
};

struct DecorationConfigureData
{
    zxdg_toplevel_decoration_v1* decoration;
    uint32_t mode;
};

class WaylandHelpers
{
    public:
    using EventListener = void (*)(WaylandEventType type, void* data);
    static void SetEventListener(EventListener listener);
    static const wl_registry_listener RegistryListener;
    static const xdg_toplevel_listener ToplevelListener;
    static const xdg_wm_base_listener WmBaseListener;
    static const zxdg_toplevel_decoration_v1_listener DecorationListener;
    static const xdg_surface_listener XdgSurfaceListener;

    private:
    static EventListener eventListener;
    static void RegistryGlobal(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t version);
    static void RegistryGlobalRemove(void* data, wl_registry* registry, uint32_t name);
    static void XdgSurfaceConfigure(void* data, xdg_surface* surface, uint32_t serial);
    static void DecorationConfigure(void* data, zxdg_toplevel_decoration_v1* decoration, uint32_t mode);
    static void WmBasePing(void* data, xdg_wm_base* base, uint32_t serial);
    static void ToplevelConfigure(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states);
    static void ToplevelClose(void* data, xdg_toplevel* toplevel);
};