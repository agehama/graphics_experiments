#pragma once

#include <vector>

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "dx.hpp"

class Mesh
{
public:

    Mesh() = default;

    template<typename VertexType, typename IndexType>
    bool init(const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices);

private:

    friend class Dx;

    template<typename VertexType>
    std::optional<D3D12_VERTEX_BUFFER_VIEW> makeVertexBuffer(const std::vector<VertexType>& vertices);

    template<typename IndexType>
    std::optional<D3D12_INDEX_BUFFER_VIEW> makeIndexBuffer(const std::vector<IndexType>& indices);

    D3D12_VERTEX_BUFFER_VIEW m_vbView;

    D3D12_INDEX_BUFFER_VIEW m_ibView;

    std::uint32_t m_verticesCount;

    std::uint32_t m_indicesCount;
};

template<typename VertexType, typename IndexType>
inline bool Mesh::init(const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices)
{
    auto vbViewOpt = makeVertexBuffer(vertices);
    if (!vbViewOpt)
    {
        return false;
    }

    auto ibViewOpt = makeIndexBuffer(indices);
    if (!ibViewOpt)
    {
        return false;
    }

    m_vbView = vbViewOpt.value();

    m_ibView = ibViewOpt.value();

    m_verticesCount = static_cast<std::uint32_t>(vertices.size());

    m_indicesCount = static_cast<std::uint32_t>(indices.size());

    return true;
}

template<typename VertexType>
inline std::optional<D3D12_VERTEX_BUFFER_VIEW> Mesh::makeVertexBuffer(const std::vector<VertexType>& vertices)
{
    const D3D12_HEAP_PROPERTIES prop =
    {
        .Type = D3D12_HEAP_TYPE_UPLOAD,
        .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
        //.CreationNodeMask
        //.VisibleNodeMask
    };

    const D3D12_RESOURCE_DESC desc =
    {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        //.Alignment
        .Width = sizeof(VertexType) * static_cast<int>(vertices.size()),
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc = {.Count = 1},
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE,
    };

    ID3D12Resource* verticesBuffer = nullptr;

    auto device = Dx::instance().device();

    CheckOpt(device->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&verticesBuffer)));

    VertexType* verticesMap = nullptr;
    CheckOpt(verticesBuffer->Map(0, nullptr, reinterpret_cast<void**>(&verticesMap)));

    std::copy(vertices.begin(), vertices.end(), verticesMap);

    verticesBuffer->Unmap(0, nullptr);

    D3D12_VERTEX_BUFFER_VIEW vbView = {
        .BufferLocation = verticesBuffer->GetGPUVirtualAddress(),
        .SizeInBytes = sizeof(VertexType) * static_cast<int>(vertices.size()),
        .StrideInBytes = sizeof(VertexType),
    };

    return vbView;
}

template<typename IndexType>
inline std::optional<D3D12_INDEX_BUFFER_VIEW> Mesh::makeIndexBuffer(const std::vector<IndexType>& indices)
{
    const D3D12_HEAP_PROPERTIES prop =
    {
        .Type = D3D12_HEAP_TYPE_UPLOAD,
        .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
        //.CreationNodeMask
        //.VisibleNodeMask
    };

    const D3D12_RESOURCE_DESC desc =
    {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        //.Alignment
        .Width = sizeof(IndexType) * static_cast<int>(indices.size()),
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc = {.Count = 1},
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE,
    };

    ID3D12Resource* indicesBuffer = nullptr;

    CheckOpt(Dx::instance().device()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indicesBuffer)));

    IndexType* indicesMap = nullptr;
    CheckOpt(indicesBuffer->Map(0, nullptr, reinterpret_cast<void**>(&indicesMap)));

    std::copy(indices.begin(), indices.end(), indicesMap);

    indicesBuffer->Unmap(0, nullptr);

    D3D12_INDEX_BUFFER_VIEW ibView = {
        .BufferLocation = indicesBuffer->GetGPUVirtualAddress(),
        .SizeInBytes = sizeof(IndexType) * static_cast<int>(indices.size()),
        .Format = DXGI_FORMAT_R32_UINT,
    };

    return ibView;
}

