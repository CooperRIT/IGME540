// Constant buffer for color tint
cbuffer ColorTint : register(b0)
{
    float4 colorTint;
    float time;
}

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
    float2 uv = input.uv;

    float3 normal = normalize(input.normal);

    float2 center = float2(0.0f, 0.0f);
    float distanceFromCenter = length(uv - center);
    float wave = sin(distanceFromCenter * 50.0f - time * 5.0f);
    
    float3 finalColor = lerp(float3(colorTint.x, 0, 0), float3(0, 0, colorTint.z), wave);

    return float4(finalColor, 1.0f);
}

