cbuffer externalData : register(b0)
{
    float aberrationAmount;
    float2 aberrationCenter; 
}

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    // Calculate direction from center
    float2 direction = input.uv - aberrationCenter;

    float2 redOffset = aberrationCenter + direction * (1.0 + aberrationAmount);
    float2 blueOffset = aberrationCenter + direction * (1.0 - aberrationAmount);
    float2 greenOffset = input.uv;

    float r = Pixels.Sample(ClampSampler, redOffset).r;
    float g = Pixels.Sample(ClampSampler, greenOffset).g;
    float b = Pixels.Sample(ClampSampler, blueOffset).b;

    return float4(r, g, b, 1.0);
}