// Constant buffer for color tint, uv offset, and UV scale
cbuffer MaterialInfo : register(b0)
{
    float4 colorTint;
    float2 uvScale;
    float2 uvOffset;
}

Texture2D SurfaceTexture : register(t0); //t = texture
SamplerState BasicSampler : register(s0); //s = sampler

// Struct representing the data we expect to receive from earlier pipeline stages
struct VertexToPixel
{
    float4 screenPosition	: SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

// The entry point for our pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
    float4 tintedColor = colorTint;

    float2 uv = input.uv * uvScale + uvOffset;

    float3 normal = normalize(input.normal);

    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, uv);

    return surfaceColor * tintedColor;
}