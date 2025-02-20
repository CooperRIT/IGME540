#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps)
{

}

std::shared_ptr<SimpleVertexShader> Material::GetVS()
{
	return std::shared_ptr<SimpleVertexShader>();
}

std::shared_ptr<SimplePixelShader> Material::GetPS()
{
	return std::shared_ptr<SimplePixelShader>();
}
