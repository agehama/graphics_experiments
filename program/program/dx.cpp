#pragma once

#include <string>
#include <optional>

#include <comdef.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#include "logger.hpp"
#include "dx.hpp"

namespace
{
    void EnableDebugLayer()
    {
        ID3D12Debug* debugLayer = nullptr;

        auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

        debugLayer->EnableDebugLayer();

        debugLayer->Release();
    }

    inline std::wstring ErrorMessage(HRESULT result)
    {
        const _com_error error(result);
        const std::wstring errorMessage(error.ErrorMessage());
        return errorMessage.empty() ? std::wstring{ L"Unknown Error" } : errorMessage;
    }
}

#define Check(result)\
{\
    if (FAILED(result))\
    {\
        ErrorLog(ErrorMessage(result));\
        return false;\
    }\
}

bool Dx::init(HWND hwnd, int width, int height)
{
    EnableDebugLayer();

    const D3D_FEATURE_LEVEL levels[] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
    };

    std::optional<D3D_FEATURE_LEVEL> featureLevel;

    for (auto level : levels)
    {
        if (D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device)) == S_OK)
        {
            featureLevel = level;
            break;
        }
    }

    if (!featureLevel)
    {
        ErrorLog(L"初期化に失敗しました");
        return false;
    }

    Check(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

    const D3D12_COMMAND_QUEUE_DESC commandQueueDesc =
    {
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
    };

    Check(m_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_commandQueue)));

    const DXGI_SWAP_CHAIN_DESC1 swapchainDesc =
    {
        .Width = static_cast<unsigned>(width),
        .Height = static_cast<unsigned>(height),
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .Stereo = false,
        .SampleDesc = {.Count = 1, .Quality = 0},
        .BufferUsage = DXGI_USAGE_BACK_BUFFER,
        .BufferCount = static_cast<unsigned>(m_backBuffers.size()),
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
    };

    Check(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory)));

    Check(m_dxgiFactory->CreateSwapChainForHwnd(m_commandQueue, hwnd, &swapchainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&m_swapChain)));

    m_backBufferHD =
    {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        .NumDescriptors = 2,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0,
    };

    Check(m_device->CreateDescriptorHeap(&m_backBufferHD, IID_PPV_ARGS(&m_backBufferHeaps)));

    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_backBufferHeaps->GetCPUDescriptorHandleForHeapStart();

    for (unsigned i = 0; i < static_cast<unsigned>(m_backBuffers.size()); ++i)
    {
        Check(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i])));

        m_device->CreateRenderTargetView(m_backBuffers[i], nullptr, handle);

        handle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    Check(m_commandAllocator->Reset());

    Check(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator, nullptr, IID_PPV_ARGS(&m_commandList)));


    return true;
}

bool Dx::frameBegin()
{
    auto currentBackBuffer = m_swapChain->GetCurrentBackBufferIndex();

    auto rtvHeap = m_backBufferHeaps->GetCPUDescriptorHandleForHeapStart();
    rtvHeap.ptr += currentBackBuffer * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    m_backBufferBD.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    m_backBufferBD.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    m_backBufferBD.Transition.pResource = m_backBuffers[currentBackBuffer];
    m_backBufferBD.Transition.Subresource = 0;

    {
        m_backBufferBD.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        m_backBufferBD.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

        m_commandList->ResourceBarrier(1, &m_backBufferBD);

        m_commandList->OMSetRenderTargets(1, &rtvHeap, true, nullptr);
    }

    m_commandList->ClearRenderTargetView(rtvHeap, m_clearColor.data(), 0, nullptr);

    {
        m_backBufferBD.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        m_backBufferBD.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

        m_commandList->ResourceBarrier(1, &m_backBufferBD);
    }

    return true;
}

bool Dx::frameEnd()
{
    m_commandList->Close();

    ID3D12CommandList* commandLists[] = { m_commandList };

    m_commandQueue->ExecuteCommandLists(1, commandLists);

    {
        ID3D12Fence* fence = nullptr;
        std::uint64_t fenceVal = 0;

        Check(m_device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

        Check(m_commandQueue->Signal(fence, ++fenceVal));

        while (fence->GetCompletedValue() != fenceVal)
        {
            auto eventHandle = CreateEvent(nullptr, false, false, nullptr);

            Check(fence->SetEventOnCompletion(fenceVal, eventHandle));

            if (eventHandle)
            {
                WaitForSingleObject(eventHandle, INFINITE);

                CloseHandle(eventHandle);
            }
        }
    }

    Check(m_commandAllocator->Reset());

    Check(m_commandList->Reset(m_commandAllocator, nullptr));

    m_swapChain->Present(1, 0);

    return true;
}
