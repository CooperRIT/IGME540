struct VertexToPixel_Sky
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

TextureCube SkyTexture : register(t0);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel_Sky input) : SV_TARGET
{
    return SkyTexture.Sample(BasicSampler, input.sampleDir);
}
