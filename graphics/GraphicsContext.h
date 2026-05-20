/**
 * @file GraphicsContext.h
 * @brief Win32 + DirectX11 + Dear ImGui overlay framework context.
 */
#pragma once

#include <Windows.h>

#include <cstdint>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

/**
 * @brief Owns the Win32 window, DirectX11 device/swapchain, and Dear ImGui initialization.
 *
 * This class provides a minimal, modular foundation for a future overlay UI layer.
 */
class GraphicsContext {
public:
    /**
     * @brief Constructs an empty graphics context.
     */
    GraphicsContext();

    /**
     * @brief Shuts down and releases all resources.
     */
    ~GraphicsContext();

    GraphicsContext(const GraphicsContext&) = delete;
    GraphicsContext& operator=(const GraphicsContext&) = delete;

    /**
     * @brief Creates a transparent click-through fullscreen overlay window and initializes DirectX11 + Dear ImGui.
     *
     * @param title Window title.
     *
     * @return True on success.
     */
    bool initialize(const wchar_t* title);

    /**
     * @brief Starts a new ImGui frame.
     */
    void beginFrame();

    /**
     * @brief Renders the current ImGui frame and presents the swapchain.
     */
    void endFrame();

    /**
     * @brief Releases ImGui, DirectX11 and Win32 resources.
     */
    void shutdown();

    /**
     * @brief Gets the created window handle.
     */
    HWND getHwnd() const;

    /**
     * @brief Shows or hides the overlay window without destroying graphics resources.
     */
    void setWindowVisible(bool visible);

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    LRESULT handleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    bool createWindow(const wchar_t* title);
    bool createDeviceD3D();
    void createRenderTarget();
    void cleanupRenderTarget();
    void cleanupDeviceD3D();

    void initImGui();
    void shutdownImGui();

    HINSTANCE hInstance = nullptr;
    HWND hwnd = nullptr;
    ATOM windowClassAtom = 0;

    IDXGISwapChain* swapChain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    ID3D11RenderTargetView* mainRenderTargetView = nullptr;

    bool imguiInitialized = false;
    bool d3dInitialized = false;
    bool windowCreated = false;
    bool visible = false;
};
