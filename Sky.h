#pragma once

#include <memory>
#include <string>
#include "Mesh.h"
#include "SimpleShader.h"
#include "Camera.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

class Sky
{
public:
    Sky(std::shared_ptr<Mesh> skyMesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, std::vector<std::wstring> textureFiles, std::shared_ptr<SimplePixelShader> ps, std::shared_ptr<SimpleVertexShader> vs);
    ~Sky();

    void Draw(Camera* camera);

private:
    // Resources
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_skyboxSRV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;

    std::shared_ptr<Mesh> m_skyMesh;
    std::shared_ptr<SimplePixelShader> m_pixelShader;
    std::shared_ptr<SimpleVertexShader> m_vertexShader;

    void CreateRenderStates();

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(std::vector<std::wstring> textureFiles);
};
