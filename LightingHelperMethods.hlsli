#ifndef Xx_SHADER_INCLUDES_xX // Each .hlsli file needs a unique identifier!
#define Xx_SHADER_INCLUDES_xX

struct VertexShaderInput
{
    float3 localPosition : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

// Struct representing the data we're sending down the pipeline
struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPosition : POSITION;
};
#endif

