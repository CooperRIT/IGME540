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

#endif
