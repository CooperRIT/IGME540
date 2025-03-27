#include "LightingHelperMethods.hlsli"

// External data constant buffer
cbuffer ExternalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    matrix worldInvTrasponse;
}

// The entry point for our vertex shader
VertexToPixel main(VertexShaderInput input)
{
    VertexToPixel output;

    matrix wvp = mul(projection, mul(view, world));

    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));

    output.uv = input.uv;
    output.normal = mul((float3x3)world, input.normal);

    return output;
}