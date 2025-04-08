#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps, DirectX::XMFLOAT4 _colorTint, float _roughness) : uvOffset(0, 0), uvScale(1, 1), roughness(0)
{
	vs = _vs;

	ps = _ps;

	colorTint = _colorTint;

	roughness = _roughness;
}

Material::Material(std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps, DirectX::XMFLOAT4 _colorTint, float _roughness, DirectX::XMFLOAT2 _uvOffset, DirectX::XMFLOAT2 _uvScale) : Material(_vs, _ps, _colorTint, _roughness)
{
	uvOffset = _uvOffset;
	uvScale = _uvScale;
}

std::shared_ptr<SimpleVertexShader> Material::GetVS()
{
	return vs;
}

std::shared_ptr<SimplePixelShader> Material::GetPS()
{
	return ps;
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return colorTint;
}

DirectX::XMFLOAT2 Material::GetUVScale()
{
	return uvScale;
}

DirectX::XMFLOAT2 Material::GetUVOffset()
{
	return uvOffset;
}

void Material::SetColorTint(DirectX::XMFLOAT4 _colorTint)
{
	colorTint = _colorTint;
}

void Material::SetVS(std::shared_ptr<SimpleVertexShader> _vs)
{
	vs = _vs;
}

void Material::SetPS(std::shared_ptr<SimplePixelShader> _ps)
{
	ps = _ps;
}

void Material::SetUVOffset(float x, float y)
{
	uvOffset = DirectX::XMFLOAT2(x, y);
}

void Material::SetUVScale(float x, float y)
{
	uvScale = DirectX::XMFLOAT2(x, y);
}

void Material::AddTextureSRV(std::string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texturePtr)
{
	textureSRVs.insert({ name, texturePtr });
}

void Material::AddSampler(std::string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerPtr)
{
	samplers.insert({ name, samplerPtr });
}

void Material::PrepareMaterial(DirectX::XMFLOAT3 cameraPos)
{
	for (auto& t : textureSRVs) 
	{ 
		ps->SetShaderResourceView(t.first.c_str(), t.second); 
	}
	for (auto& s : samplers) 
	{ 
		ps->SetSamplerState(s.first.c_str(), s.second); 
	}

	ps->SetFloat3("cameraPos", cameraPos);
	ps->SetFloat("roughness", roughness);
}

void Material::PrepareLight(std::vector<Light> lights)
{
	/*for (int i = 0; i < lights.size(); i++)
	{
		switch (lights[i].Type)
		{
			case 0:
				ps->SetData("directionalLight", &lights[i], sizeof(Light));
				break;
		}
	}*/


	//temp for setting ambient light
	ps->SetFloat3("ambientLightColor", DirectX::XMFLOAT3(1, .81f, 0.87f));


	ps->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
}


