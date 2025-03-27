#include "LightingHelperMethods.hlsli"

// Constant buffer for color tint, uv offset, and UV scale
cbuffer MaterialInfo : register(b0)
{
    float4 colorTint;
    float2 uvScale;
    float2 uvOffset;
    float3 cameraPos;
    float roughness;
    float3 ambientLightColor;
}

Texture2D SurfaceTexture : register(t0); //t = texture
SamplerState BasicSampler : register(s0); //s = sampler


// The entry point for our pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
    float4 tintedColor = colorTint;

    float2 uv = input.uv * uvScale + uvOffset;

    float3 normal = normalize(input.normal);

    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, uv);

    return surfaceColor * tintedColor * ambientLightColor.xyzz;
}