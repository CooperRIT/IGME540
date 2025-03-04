#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> _vs, std::shared_ptr<SimplePixelShader> _ps, DirectX::XMFLOAT4 _colorTint)
{
	vs = _vs;

	ps = _ps;

	colorTint = _colorTint;
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

void Material::SetVS(std::shared_ptr<SimpleVertexShader> _vs)
{
	vs = _vs;
}

void Material::SetPS(std::shared_ptr<SimplePixelShader> _ps)
{
	ps = _ps;
}
