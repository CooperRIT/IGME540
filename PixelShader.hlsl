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

//PBR
Texture2D Albedo : register(t0);
Texture2D NormalMapTexture : register(t1);
Texture2D RoughnessMapTexture : register(t2);
Texture2D MetalnessMapTexture : register(t3);
Texture2D ShadowMap : register(t4);
SamplerState BasicSampler : register(s0);
SamplerComparisonState ShadowSampler : register(s1);


float4 main(VertexToPixel input) : SV_TARGET
{
    // Perform the perspective divide (divide by W) ourselves
    input.shadowMapPos /= input.shadowMapPos.w;
    // Convert the normalized device coordinates to UVs for sampling
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y; // Flip the Y
    // Grab the distances we need: light-to-pixel and closest-surface
    float distToLight = input.shadowMapPos.z;
    float distShadowMap = ShadowMap.Sample(BasicSampler, shadowUV).r;
    // For testing, just return black where there are shadows.
    float shadowAmount = ShadowMap.SampleCmpLevelZero(
        ShadowSampler,
        shadowUV,
        distToLight).r;

    float3 N = normalize(input.normal);
    float2 uv = input.uv * uvScale + uvOffset;

    // Sample textures
    float4 albedoColor = pow(Albedo.Sample(BasicSampler, uv), 2.2f);
    float roughness = RoughnessMapTexture.Sample(BasicSampler, uv).r;
    float metalness = MetalnessMapTexture.Sample(BasicSampler, uv).r;

    // Sample and unpack normal map
    float3 unpackedNormal = NormalMapTexture.Sample(BasicSampler, uv).rgb * 2.0f - 1.0f;
    unpackedNormal = normalize(unpackedNormal);

    // Build TBN matrix
    float3 T = normalize(input.tangent);
    T = normalize(T - N * dot(T, N));
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    N = normalize(mul(unpackedNormal, TBN));

    // Determine specular color based on metalness
    float3 specularColor = lerp(F0_NON_METAL, albedoColor.rgb, metalness);

    // View direction
    float3 V = normalize(cameraPos - input.worldPosition);

    float4 totalLight = float4(0, 0, 0, 0);

    for (int i = 0; i < 5; i++)
    {
        if (lights[i].Intensity <= 0) continue;

        float3 L = float3(0, 0, 0);
        float attenuation = 1.0f;

        // Calculate light direction and attenuation based on type
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                L = normalize(-lights[i].Direction);
                break;

            case LIGHT_TYPE_POINT:
                L = normalize(lights[i].Position - input.worldPosition);
                attenuation = Attenuate(lights[i], input.worldPosition);
                break;

            case LIGHT_TYPE_SPOT:
                L = normalize(lights[i].Position - input.worldPosition);
                attenuation = Attenuate(lights[i], input.worldPosition);
                attenuation *= SpotAttenuate(lights[i], input.worldPosition);
                break;
        }

        // Calculate lighting components
        float3 F;
        float diffuse = DiffusePBR(N, L);
        float3 specular = MicrofacetBRDF(N, L, V, roughness, specularColor, F);

        // Apply energy conservation
        float3 balancedDiff = DiffuseEnergyConserve(diffuse, F, metalness);

        // Combine results
        float3 surfaceColor = lerp(albedoColor.rgb, float3(0, 0, 0), metalness);
        float shadowFactor = (lights[i].Type == LIGHT_TYPE_DIRECTIONAL) ? shadowAmount : 1.0f;
        float3 lightResult = (balancedDiff * surfaceColor + specular) * lights[i].Intensity * lights[i].Color * shadowFactor;

        totalLight += float4(lightResult * attenuation, 0);
    }

    // Apply color tint and gamma correction
    float4 finalColor = totalLight * colorTint;
    finalColor.rgb = pow(finalColor.rgb, 1.0 / 2.2);

    return finalColor;
}
