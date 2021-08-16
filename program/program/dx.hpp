#pragma once

#include <array>

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>

class Dx
{
public:

    Dx() = default;

    bool init(HWND hwnd, int width, int height);

    bool frameBegin();

    bool frameEnd();

private:

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
};
