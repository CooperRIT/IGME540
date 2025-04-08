#include "Sky.h"
#include "Graphics.h"
#include "Camera.h"
#include <WICTextureLoader.h>

using namespace DirectX;

Sky::Sky(std::shared_ptr<Mesh> skyMesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, std::vector<std::wstring> textureFiles, std::shared_ptr<SimplePixelShader> ps, std::shared_ptr<SimpleVertexShader> vs)
{
	m_skyMesh = skyMesh;

    // Load the cube map texture
	m_skyboxSRV = CreateCubemap(textureFiles);

	m_samplerState = samplerState;

    // Create shaders
	m_pixelShader = ps;
	m_vertexShader = vs;

    // Create render states
    CreateRenderStates();
}

Sky::~Sky()
{
    
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Sky::CreateCubemap(std::vector<std::wstring> textureFiles)
{
	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not SHADER RESOURCE VIEWS!
	// - Explicitly NOT generating mipmaps, as we don't need them for the sky!
	// - Order matters here!  +X, -X, +Y, -Y, +Z, -Z
	Microsoft::WRL::ComPtr<ID3D11Texture2D> textures[6] = {};
	CreateWICTextureFromFile(Graphics::Device.Get(), textureFiles[0].c_str(), (ID3D11Resource**)textures[0].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), textureFiles[1].c_str(), (ID3D11Resource**)textures[1].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), textureFiles[2].c_str(), (ID3D11Resource**)textures[2].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), textureFiles[3].c_str(), (ID3D11Resource**)textures[3].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), textureFiles[4].c_str(), (ID3D11Resource**)textures[4].GetAddressOf(), 0);
	CreateWICTextureFromFile(Graphics::Device.Get(), textureFiles[5].c_str(), (ID3D11Resource**)textures[5].GetAddressOf(), 0);

	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first texture
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);

	// Describe the resource for the cube map, which is simply 
	// a "texture 2d array" with the TEXTURECUBE flag set.  
	// This is a special GPU resource format, NOT just a 
	// C++ array of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6;            // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0;       // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width;   // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1;            // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // This should be treated as a CUBE, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;

	// Create the final texture resource to hold the cube map
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubeMapTexture;
	Graphics::Device->CreateTexture2D(&cubeDesc, 0, cubeMapTexture.GetAddressOf());

	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0,  // Which mip (zero, since there's only one)
			i,  // Which array element?
			1); // How many mip levels are in the texture?

		// Copy from one resource (texture) to another
		Graphics::Context->CopySubresourceRegion(
			cubeMapTexture.Get(),  // Destination resource
			subresource,           // Dest subresource index (one of the array elements)
			0, 0, 0,               // XYZ location of copy
			textures[i].Get(),     // Source resource
			0,                     // Source subresource index (we're assuming there's only one)
			0);                    // Source subresource "box" of data to copy (zero means the whole thing)
	}

	// At this point, all of the faces have been copied into the 
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format;         // Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1;        // Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0;  // Index of the first mip we want to see

	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	Graphics::Device->CreateShaderResourceView(cubeMapTexture.Get(), &srvDesc, cubeSRV.GetAddressOf());

	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
}

void Sky::CreateRenderStates()
{
    // Create the rasterizer state to render the inside of the cube (skybox)
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_FRONT; // Cull front faces to draw the inside
	rasterDesc.DepthClipEnable = true;
    Graphics::Device.Get()->CreateRasterizerState(&rasterDesc, m_rasterizerState.GetAddressOf());

    // Create the depth stencil state
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    Graphics::Device.Get()->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf());
}

void Sky::Draw(Camera* camera)
{
    // Set render states
    Graphics::Context->RSSetState(m_rasterizerState.Get());
    Graphics::Context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

    // Prepare the sky shaders
    m_vertexShader->SetShader();
    m_pixelShader->SetShader();

    // Set the appropriate sampler and SRV for the pixel shader
	m_pixelShader->SetShaderResourceView("SkyTexture", m_skyboxSRV);
    m_pixelShader->SetSamplerState("BasicSampler", m_samplerState);
	m_pixelShader->CopyAllBufferData();

    // Set view and projection matrices
    m_vertexShader->SetMatrix4x4("view", camera->GetView());
    m_vertexShader->SetMatrix4x4("projection", camera->GetProjection());
    m_vertexShader->CopyAllBufferData();

    // Draw the sky mesh
    m_skyMesh->Draw();

    // Reset render states
    Graphics::Context->RSSetState(nullptr);
    Graphics::Context->OMSetDepthStencilState(nullptr, 0);
}