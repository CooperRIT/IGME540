// External data constant buffer
cbuffer ExternalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
}

// Struct representing a single vertex worth of data
struct VertexShaderInput
{
    float3 localPosition	: POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

// Struct representing the data we're sending down the pipeline
struct VertexToPixel
{
    float4 screenPosition	: SV_POSITION;
    float2 uv : TEXCOORD; 
    float3 normal : NORMAL;
};

// The entry point for our vertex shader
VertexToPixel main(VertexShaderInput input)
{
    VertexToPixel output;

    matrix wvp = mul(projection, mul(view, world));

    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));

    output.uv = input.uv;
    output.normal = input.normal;

    return output;
}