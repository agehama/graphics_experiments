#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "dx.hpp"

class ShaderPipeline
{
public:

    ShaderPipeline() = default;

    bool init(const std::wstring& vertexShaderPath, const std::wstring& pixelShaderPath);

private:

    friend class Dx;

    ID3D12PipelineState* m_pipelineState = nullptr;

    ID3D12RootSignature* m_rootSignature = nullptr;
};
