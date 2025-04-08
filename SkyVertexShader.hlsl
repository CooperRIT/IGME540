// Vertex shader input structure
struct VertexShaderInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

struct VertexToPixel_Sky
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

cbuffer ViewProjection : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

VertexToPixel_Sky main(VertexShaderInput input)
{
    VertexToPixel_Sky output;

    // Remove the translation from the view matrix to prevent sky from moving with the camera
    float4x4 viewNoTranslation = view;
    viewNoTranslation._14 = 0.0f;
    viewNoTranslation._24 = 0.0f;
    viewNoTranslation._34 = 0.0f;

    matrix vp = mul(projection, viewNoTranslation);
    output.position = mul(vp, float4(input.position, 1.0f));

    output.position.z = output.position.w;

    output.sampleDir = normalize(input.position);

    return output;
}
