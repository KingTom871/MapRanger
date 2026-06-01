#include "graphics/GraphicsContext.h"

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_dx11.h"
#include "ImGui/backends/imgui_impl_win32.h"
#include "utils/Profiler.h"

#include <d3d11.h>
#include <dxgi.h>

#include <iostream>
#include <iterator>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

namespace {
constexpr const wchar_t* kWindowClassName = L"MapRangerWindowClass";
constexpr const char* kFontPath = "C:\\Windows\\Fonts\\msyh.ttc";
constexpr float kFontSize = 20.0f;
constexpr float kStyleScale = 1.5f;
}

GraphicsContext::GraphicsContext() = default;

GraphicsContext::~GraphicsContext() {
    shutdown();
}

bool GraphicsContext::initialize(const wchar_t* title) {
    shutdown();

    if (!createWindow(title)) {
        std::cerr << "[Graphics] Failed to create Win32 window\n";
        shutdown();
        return false;
    }

    if (!createDeviceD3D()) {
        std::cerr << "[Graphics] Failed to create D3D11 device\n";
        shutdown();
        return false;
    }

    initImGui();
    return true;
}

void GraphicsContext::beginFrame() {
    if (!imguiInitialized) {
        return;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void GraphicsContext::endFrame() {
    if (!imguiInitialized || !d3dInitialized) {
        return;
    }

    {
        ScopedTimer timer("render");
        ImGui::Render();

        const float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        ID3D11RenderTargetView* renderTargetView = mainRenderTargetView.Get();
        deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
        deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    {
        ScopedTimer timer("present");
        swapChain->Present(1, 0);
    }
}

void GraphicsContext::shutdown() {
    shutdownImGui();
    mainRenderTargetView.Reset();
    swapChain.Reset();
    deviceContext.Reset();
    device.Reset();
    d3dInitialized = false;

    if (hwnd != nullptr) {
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }

    if (windowClassAtom != 0) {
        UnregisterClassW(kWindowClassName, hInstance);
        windowClassAtom = 0;
    }

    visible = false;
    hInstance = nullptr;
}

HWND GraphicsContext::getHwnd() const {
    return hwnd;
}

void GraphicsContext::setWindowVisible(bool nextVisible) {
    if (hwnd == nullptr || visible == nextVisible) {
        return;
    }

    ShowWindow(hwnd, nextVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
    visible = nextVisible;
}

bool GraphicsContext::createWindow(const wchar_t* title) {
    hInstance = GetModuleHandleW(nullptr);
    if (hInstance == nullptr) {
        std::cerr << "[Graphics] GetModuleHandleW failed (error " << GetLastError() << ")\n";
        return false;
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = &GraphicsContext::WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = nullptr;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = kWindowClassName;
    wc.hIconSm = nullptr;

    windowClassAtom = RegisterClassExW(&wc);
    if (windowClassAtom == 0) {
        std::cerr << "[Graphics] RegisterClassExW failed (error " << GetLastError() << ")\n";
        return false;
    }

    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        wc.lpszClassName,
        title,
        WS_POPUP,
        0,
        0,
        screenWidth,
        screenHeight,
        nullptr,
        nullptr,
        hInstance,
        this
    );

    if (hwnd == nullptr) {
        std::cerr << "[Graphics] CreateWindowExW failed (error " << GetLastError() << ")\n";
        return false;
    }

    if (SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY) == FALSE) {
        std::cerr << "[Graphics] SetLayeredWindowAttributes failed (error " << GetLastError() << ")\n";
        DestroyWindow(hwnd);
        hwnd = nullptr;
        return false;
    }

    SetWindowPos(
        hwnd,
        HWND_TOPMOST,
        0,
        0,
        screenWidth,
        screenHeight,
        SWP_SHOWWINDOW | SWP_NOACTIVATE
    );
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    UpdateWindow(hwnd);

    visible = true;
    return true;
}

bool GraphicsContext::createDeviceD3D() {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };

    D3D_FEATURE_LEVEL featureLevel{};
    const HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        static_cast<UINT>(std::size(featureLevels)),
        D3D11_SDK_VERSION,
        &sd,
        swapChain.GetAddressOf(),
        device.GetAddressOf(),
        &featureLevel,
        deviceContext.GetAddressOf()
    );

    if (FAILED(hr)) {
        std::cerr << "[Graphics] D3D11CreateDeviceAndSwapChain failed (hr=0x" << std::hex << hr << std::dec << ")\n";
        return false;
    }

    if (!createRenderTarget()) {
        return false;
    }

    d3dInitialized = true;
    return true;
}

bool GraphicsContext::createRenderTarget() {
    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)))) {
        std::cerr << "[Graphics] IDXGISwapChain::GetBuffer failed\n";
        return false;
    }

    mainRenderTargetView.Reset();
    const HRESULT hr = device->CreateRenderTargetView(backBuffer, nullptr, mainRenderTargetView.GetAddressOf());
    backBuffer->Release();
    if (FAILED(hr)) {
        std::cerr << "[Graphics] ID3D11Device::CreateRenderTargetView failed (hr=0x" << std::hex << hr << std::dec << ")\n";
        return false;
    }

    return true;
}

void GraphicsContext::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    io.Fonts->AddFontFromFileTTF(
        kFontPath,
        kFontSize,
        nullptr,
        io.Fonts->GetGlyphRangesChineseFull()
    );

    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(kStyleScale);

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());

    imguiInitialized = true;
}

void GraphicsContext::shutdownImGui() {
    if (!imguiInitialized) {
        return;
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    imguiInitialized = false;
}

LRESULT CALLBACK GraphicsContext::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_NCHITTEST) {
        return HTTRANSPARENT;
    }

    if (ImGui::GetCurrentContext() != nullptr && ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return TRUE;
    }

    GraphicsContext* self = nullptr;

    if (msg == WM_NCCREATE) {
        const auto* cs = reinterpret_cast<CREATESTRUCTW*>(lparam);
        self = static_cast<GraphicsContext*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    } else {
        self = reinterpret_cast<GraphicsContext*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (self != nullptr) {
        return self->handleMessage(hwnd, msg, wparam, lparam);
    }

    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

LRESULT GraphicsContext::handleMessage(HWND hwndHandle, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_SIZE:
        if (d3dInitialized && swapChain && wparam != SIZE_MINIMIZED) {
            mainRenderTargetView.Reset();
            const HRESULT hr = swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
            if (FAILED(hr)) {
                std::cerr << "[Graphics] IDXGISwapChain::ResizeBuffers failed (hr=0x" << std::hex << hr << std::dec << ")\n";
                return 0;
            }

            if (!createRenderTarget()) {
                return 0;
            }
        }
        return 0;

    case WM_SYSCOMMAND:
        if ((wparam & 0xFFF0) == SC_KEYMENU) {
            return 0;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        break;
    }

    return DefWindowProcW(hwndHandle, msg, wparam, lparam);
}
