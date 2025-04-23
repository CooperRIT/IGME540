#ifndef Xx_SHADER_INCLUDES_xX // Each .hlsli file needs a unique identifier!
#define Xx_SHADER_INCLUDES_xX

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

#define MAX_SPECULAR_EXPONENT 256.0f

struct VertexShaderInput
{
    float3 localPosition : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

// Struct representing the data we're sending down the pipeline
struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPosition : POSITION;
    float3 tangent : TANGENT;
    float4 shadowMapPos : SHADOW_POSITION;
};

struct Light
{
    int Type; // Which kind of light? 0, 1 or 2 (see above)
    float3 Direction; // Directional and Spot lights need a direction
    float Range; // Point and Spot lights have a max range for attenuation
    float3 Position; // Point and Spot lights have a position in space
    float Intensity; // All lights need an intensity
    float3 Color; // All lights need a color
    float SpotInnerAngle; // Inner cone angle (in radians) – Inside this, full light!
    float SpotOuterAngle; // Outer cone angle (radians) – Outside this, no light!
    float2 Padding; // Purposefully padding to hit the 16-byte boundary
};

float3 CalculateDiffuse(float3 normal, float3 lightDir, Light light)
{
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    return light.Color * light.Intensity * diffuseFactor;
}

float3 CalculateSpecular(float3 normal, float3 lightDir, float3 viewDir, Light light, float roughness)
{
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    if (specExponent < 0.05f)
    {
        return float3(0.0, 0.0, 0.0);
    }
    float3 reflection = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflection, viewDir), 0.0), specExponent);
    return light.Color * light.Intensity * spec;
}

// Main calculation for directional light
float4 CalculateDirectionalLight(VertexToPixel input, Light light, float3 cameraPos, float roughness, float3 customNormal)
{
    float3 lightDir = normalize(-light.Direction);
    float3 normal = normalize(customNormal);
    float3 viewDir = normalize(cameraPos - input.worldPosition);

    float3 diffuseColor = CalculateDiffuse(normal, lightDir, light);
    float3 specularColor = CalculateSpecular(normal, lightDir, viewDir, light, roughness);

    float3 totalLight = diffuseColor + specularColor;

    return float4(totalLight, 1.0);
}

float Attenuate(Light light, float3 worldPosition)
{
    float dist = distance(light.Position, worldPosition);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

// Main calculation for point lighting 
float4 CalculatePointLighting(VertexToPixel input, Light light, float3 cameraPos, float roughness, float3 customNormal)
{
    float3 lightDir = normalize(light.Position - input.worldPosition);
    float3 normal = normalize(customNormal);
    float3 viewDir = normalize(cameraPos - input.worldPosition);

    float3 diffuseColor = CalculateDiffuse(normal, lightDir, light);
    float3 specularColor = CalculateSpecular(normal, lightDir, viewDir, light, roughness);

    float attenuation = Attenuate(light, input.worldPosition);
    diffuseColor *= attenuation;
    specularColor *= attenuation;

    float3 totalLight = diffuseColor + specularColor;

    return float4(totalLight, 1.0);
}

// Spotlight cone attenuation based on angles
float SpotAttenuate(Light light, float3 worldPos)
{
    float3 lightToPixel = normalize(worldPos - light.Position);
    float3 spotDirection = normalize(light.Direction);

    float cosAngle = dot(lightToPixel, spotDirection);

    float cosInner = cos(light.SpotInnerAngle);
    float cosOuter = cos(light.SpotOuterAngle);

    if (cosAngle >= cosInner)
    {
        return 1.0f;
    }
    else if (cosAngle <= cosOuter)
    {
        return 0.0f;
    }
    else
    {
        float attenuation = saturate((cosAngle - cosOuter) / (cosInner - cosOuter));
        return attenuation;
    }
}


// Main calculation for spotlight
float4 CalculateSpotlightLighting(VertexToPixel input, Light light, float3 cameraPos, float roughness, float3 customNormal)
{
    float3 lightDir = normalize(light.Position - input.worldPosition);
    float3 normal = normalize(customNormal);
    float3 viewDir = normalize(cameraPos - input.worldPosition);

    float3 diffuseColor = CalculateDiffuse(normal, lightDir, light);
    float3 specularColor = CalculateSpecular(normal, lightDir, viewDir, light, roughness);

    float attenuation = Attenuate(light, input.worldPosition);
    attenuation *= SpotAttenuate(light, input.worldPosition);

    diffuseColor *= attenuation;
    specularColor *= attenuation;

    return float4(diffuseColor + specularColor, 1);
}

//PBR

// PBR Helper Functions
static const float PI = 3.14159265359;
static const float3 F0_NON_METAL = float3(0.04, 0.04, 0.04);

// Fresnel approximation
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Normal Distribution Function (GGX/Trowbridge-Reitz)
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

// Geometry Function (Schlick GGX)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

// Smith's method for geometry
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Microfacet BRDF (Cook-Torrance)
float3 MicrofacetBRDF(float3 N, float3 L, float3 V, float roughness, float3 F0, out float3 F_out)
{
    float3 H = normalize(V + L);

    // Calculate each component
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
    F_out = F;

    // Combine terms
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    float3 specular = numerator / max(denominator, 0.001);

    return specular;
}

// Energy conservation for diffuse
float3 DiffuseEnergyConserve(float diffuse, float3 F, float metalness)
{
    return diffuse * (1.0 - F) * (1.0 - metalness);
}

// PBR Diffuse calculation
float DiffusePBR(float3 normal, float3 lightDir)
{
    return max(dot(normal, lightDir), 0.0);
}

#endif
