// Constant buffer for color tint
cbuffer ColorTint : register(b0)
{
    float4 colorTint;
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

    return float4(uv, 0, 0);
}