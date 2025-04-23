#include "LightingHelperMethods.hlsli"

// External data constant buffer
cbuffer ExternalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    matrix worldInvTranspose;
    matrix lightView;
    matrix lightProjection;
}

// The entry point for our vertex shader
VertexToPixel main(VertexShaderInput input)
{
    VertexToPixel output;

    matrix wvp = mul(projection, mul(view, world));

    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));

    output.uv = input.uv;

    output.normal = mul((float3x3)worldInvTranspose, input.normal);

    output.worldPosition = mul(world, float4(input.localPosition, 1)).xyz;

    output.tangent = mul((float3x3)world, input.tangent);

    //Shadow Map Calculations
    matrix shadowWVP = mul(lightProjection, mul(lightView, world));

    output.shadowMapPos = mul(shadowWVP, float4(input.localPosition, 1.0f));

    return output;
}