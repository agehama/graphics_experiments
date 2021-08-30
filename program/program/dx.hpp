#pragma once

#include <array>
#include <optional>

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>

class Mesh;

class ShaderPipeline;

class Dx
{
public:

    static Dx& instance()
    {
        static Dx i;
        return i;
    }

    bool init(HWND hwnd, int width, int height);

    bool frameBegin();

    bool frameEnd();

    ID3D12Device* device() { return m_device; }

    void setPipeline(const ShaderPipeline& pipeline);

    void draw(const Mesh& mesh);

private:

    Dx() = default;

    ID3D12Device* m_device = nullptr;

    IDXGISwapChain4* m_swapChain = nullptr;

    ID3D12CommandAllocator* m_commandAllocator = nullptr;

    ID3D12GraphicsCommandList* m_commandList = nullptr;

    ID3D12CommandQueue* m_commandQueue = nullptr;

    IDXGIFactory6* m_dxgiFactory = nullptr;

    std::array<ID3D12Resource*, 2> m_backBuffers;

    D3D12_DESCRIPTOR_HEAP_DESC  m_backBufferHD = {};

    D3D12_RESOURCE_BARRIER      m_backBufferBD = {};

    ID3D12DescriptorHeap* m_backBufferHeaps = nullptr;

    std::array<float, 4> m_clearColor = { 0.f,0.f,0.f,1.f };

    D3D12_VIEWPORT m_windowViewport = {};

    D3D12_RECT m_scissorRect = {};

    D3D12_PRIMITIVE_TOPOLOGY m_primitiveTOpology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
