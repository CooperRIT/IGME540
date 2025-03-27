#include "LightingHelperMethods.hlsli"

// Constant buffer for color tint
cbuffer ColorTint : register(b0)
{
    float4 colorTint;
}

// The entry point for our pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
    float2 uv = input.uv;

    float3 normal = normalize(input.normal);

    return float4(normal, 0);
}