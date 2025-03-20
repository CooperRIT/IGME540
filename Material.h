#pragma once

#include "SimpleShader.h"
#include <memory>
#include <DirectXMath.h>


class Material
{

public:
	Material(std::shared_ptr<SimpleVertexShader> _vs,
		std::shared_ptr<SimplePixelShader> _ps,
		DirectX::XMFLOAT4 _colorTint);

	Material(std::shared_ptr<SimpleVertexShader> _vs,
		std::shared_ptr<SimplePixelShader> _ps,
		DirectX::XMFLOAT4 _colorTint,
		DirectX::XMFLOAT2 _uvOffset,
		DirectX::XMFLOAT2 _uvScale);

	std::shared_ptr<SimpleVertexShader> GetVS();
	std::shared_ptr<SimplePixelShader> GetPS();
	DirectX::XMFLOAT4 GetColorTint();
	DirectX::XMFLOAT2 GetUVScale();
	DirectX::XMFLOAT2 GetUVOffset();

	void SetColorTint(DirectX::XMFLOAT4 _colorTint);
	void SetVS(std::shared_ptr<SimpleVertexShader> _vs);
	void SetPS(std::shared_ptr<SimplePixelShader> _ps);
	void SetUVOffset(float x, float y);
	void SetUVScale(float x, float y);

	void AddTextureSRV(std:: string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texturePtr);
	void AddSampler(std::string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerPtr);

	void PrepareMaterial();

private:
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;
	DirectX::XMFLOAT4 colorTint;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

	//texture info
	DirectX::XMFLOAT2 uvOffset;
	DirectX::XMFLOAT2 uvScale;

};

