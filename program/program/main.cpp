#include <DirectXMath.h>

#include "logger.hpp"
#include "window.hpp"
#include "dx.hpp"
#include "mesh.hpp"
#include "shaderPipeline.hpp"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    ClearLog();

    Window window(1600, 900, L"program");

    auto& dx = Dx::instance();

    if (!dx.init(window.hwnd(), window.width(), window.height()))
    {
        return 1;
    }

    std::vector<DirectX::XMFLOAT3> vertices(
        {
            DirectX::XMFLOAT3(-1.f,-1.f,0.f),
            DirectX::XMFLOAT3(-1.f,1.f,0.f),
            DirectX::XMFLOAT3(1.f,-1.f,0.f),
        }
    );

    std::vector<int> indices({ 0,1,2 });

    window.update();

    Mesh mesh;
    if (!mesh.init(vertices, indices))
    {
        return 1;
    }

    ShaderPipeline pipeline;
    if (!pipeline.init(L"shader/vs.hlsl", L"shader/ps.hlsl"))
    {
        return 1;
    }
    
    while (window.update())
    {
        if (!dx.frameBegin())
        {
            return 1;
        }

        dx.setPipeline(pipeline);

        dx.draw(mesh);

        if (!dx.frameEnd())
        {
            return 1;
        }
    }

    return 0;
}
