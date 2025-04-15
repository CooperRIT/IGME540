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
    Light lights[5];
}

Texture2D SurfaceTexture : register(t0);
SamplerState BasicSampler : register(s0);
Texture2D NormalMapTexture : register(t1);

// The entry point for our pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
    float3 N = normalize(input.normal);

    float2 uv = input.uv * uvScale + uvOffset;

    // Sample base color texture
    float4 surfaceColor = pow(SurfaceTexture.Sample(BasicSampler, uv), 2.2f);

    float3 finalNormal = N;

    // Sample and unpack the normal map
    float3 unpackedNormal = NormalMapTexture.Sample(BasicSampler, uv).rgb * 2.0f - 1.0f;
    unpackedNormal = normalize(unpackedNormal);

    // Build TBN matrix
    float3 T = normalize(input.tangent);
    T = normalize(T - N * dot(T, N));
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);

    finalNormal = normalize(mul(unpackedNormal, TBN));

    float4 totalLight = float4(0, 0, 0, 0);


    for (int i = 0; i < 5; i++)
    {
        switch (lights[i].Type)
        {
            case 0:
                totalLight += CalculateDirectionalLight(input, lights[i], cameraPos, roughness, finalNormal);
                break;
            case 1:
                totalLight += CalculatePointLighting(input, lights[i], cameraPos, roughness, finalNormal);
                break;
            case 2:
                totalLight += CalculateSpotlightLighting(input, lights[i], cameraPos, roughness, finalNormal);
                break;
        }
    }

    totalLight += float4(ambientLightColor, 1);

    float4 totalColor = totalLight * colorTint * surfaceColor;

    return float4(pow(totalColor.xyz, 1.0f/2.2f), 1);

}
