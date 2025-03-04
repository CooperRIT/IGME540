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

	std::shared_ptr<SimpleVertexShader> GetVS();
	std::shared_ptr<SimplePixelShader> GetPS();
	DirectX::XMFLOAT4 GetColorTint();

	void SetVS(std::shared_ptr<SimpleVertexShader> _vs);
	void SetPS(std::shared_ptr<SimplePixelShader> _ps);

private:
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;
	DirectX::XMFLOAT4 colorTint;
};

