#include <string>
#include <optional>

#include <comdef.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#include <d3dcompiler.h>

#include "logger.hpp"
#include "dx.hpp"
#include "mesh.hpp"
#include "shaderPipeline.hpp"

bool ShaderPipeline::init(const std::wstring& vertexShaderPath, const std::wstring& pixelShaderPath)
{
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    ID3DBlob* errorBlob = nullptr;

    HRESULT shaderResult;

    shaderResult = D3DCompileFromFile(
        vertexShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VS", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vsBlob, &errorBlob);

    if (FAILED(shaderResult))
    {
        if (shaderResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            ErrorLog(L"ERROR_FILE_NOT_FOUND");
        }
        else
        {
            ErrorLog(ErrorMessage(shaderResult));
        }

        return false;
    }

    shaderResult = D3DCompileFromFile(
        pixelShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PS", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &psBlob, &errorBlob);

    if (FAILED(shaderResult))
    {
        if (shaderResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            ErrorLog(L"ERROR_FILE_NOT_FOUND");
        }
        else
        {
            ErrorLog(ErrorMessage(shaderResult));
        }

        return false;
    }

    D3D12_INPUT_ELEMENT_DESC inputLayout[] =
    {
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }
    };

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* rootSigBlob = nullptr;

    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);

    auto device = Dx::instance().device();

    device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
    rootSigBlob->Release();

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};

    pipelineDesc.pRootSignature = m_rootSignature;
    pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
    pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
    pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
    pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

    pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    pipelineDesc.RasterizerState.MultisampleEnable = false;
    pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    pipelineDesc.RasterizerState.DepthClipEnable = true;

    pipelineDesc.BlendState.AlphaToCoverageEnable = false;
    pipelineDesc.BlendState.IndependentBlendEnable = false;

    D3D12_RENDER_TARGET_BLEND_DESC blendDesc = {};

    blendDesc.BlendEnable = false;
    blendDesc.LogicOpEnable = false;
    blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    pipelineDesc.BlendState.RenderTarget[0] = blendDesc;


    pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
    pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

    pipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    pipelineDesc.NumRenderTargets = 1;
    pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    pipelineDesc.SampleDesc.Count = 1;
    pipelineDesc.SampleDesc.Quality = 0;

    Check(device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_pipelineState)));

    return true;
}
