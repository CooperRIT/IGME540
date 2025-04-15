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

//PBR LIGHTING FUNCTIONS

// CONSTANTS ===================

// Make sure to place these at the top of your shader(s) or shader include file
// - You don�t necessarily have to keep all the comments; they�re here for your reference

// A constant Fresnel value for non-metals (glass and plastic have values of about 0.04)
static const float F0_NON_METAL = 0.04f;

// Minimum roughness for when spec distribution function denominator goes to zero
static const float MIN_ROUGHNESS = 0.0000001f; // 6 zeros after decimal

// Handy to have this as a constant
static const float PI = 3.14159265359f;




// PBR FUNCTIONS ================

// Lambert diffuse BRDF - Same as the basic lighting diffuse calculation!
// - NOTE: this function assumes the vectors are already NORMALIZED!
float DiffusePBR(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}



// Calculates diffuse amount based on energy conservation
//
// diffuse   - Diffuse amount
// F         - Fresnel result from microfacet BRDF
// metalness - surface metalness amount 
float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
    return diffuse * (1 - F) * (1 - metalness);
}
 


// Normal Distribution Function: GGX (Trowbridge-Reitz)
//
// a - Roughness
// h - Half vector
// n - Normal
// 
// D(h, n, a) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float D_GGX(float3 n, float3 h, float roughness)
{
	// Pre-calculations
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness;
    float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!

	// ((n dot h)^2 * (a^2 - 1) + 1)
	// Can go to zero if roughness is 0 and NdotH is 1
    float denomToSquare = NdotH2 * (a2 - 1) + 1;

	// Final value
    return a2 / (PI * denomToSquare * denomToSquare);
}



// Fresnel term - Schlick approx.
// 
// v - View vector
// h - Half vector
// f0 - Value when l = n
//
// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 F_Schlick(float3 v, float3 h, float3 f0)
{
	// Pre-calculations
    float VdotH = saturate(dot(v, h));

	// Final value
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}



// Geometric Shadowing - Schlick-GGX
// - k is remapped to a / 2, roughness remapped to (r+1)/2 before squaring!
//
// n - Normal
// v - View vector
//
// G_Schlick(n,v,a) = (n dot v) / ((n dot v) * (1 - k) * k)
//
// Full G(n,v,l,a) term = G_SchlickGGX(n,v,a) * G_SchlickGGX(n,l,a)
float G_SchlickGGX(float3 n, float3 v, float roughness)
{
	// End result of remapping:
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));

	// Final value
	// Note: Numerator should be NdotV (or NdotL depending on parameters).
	// However, these are also in the BRDF's denominator, so they'll cancel!
	// We're leaving them out here AND in the BRDF function as the
	// dot products can get VERY small and cause rounding errors.
    return 1 / (NdotV * (1 - k) + k);
}


 
// Cook-Torrance Microfacet BRDF (Specular)
//
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
// - parts of the denominator are canceled out by numerator (see below)
//
// D() - Normal Distribution Function - Trowbridge-Reitz (GGX)
// F() - Fresnel - Schlick approx
// G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0, out float3 F_out)
{
	// Other vectors
    float3 h = normalize(v + l);

	// Run numerator functions
    float D = D_GGX(n, h, roughness);
    float3 F = F_Schlick(v, h, f0);
    float G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);
	
	// Pass F out of the function for diffuse balance
    F_out = F;

	// Final specular formula
	// Note: The denominator SHOULD contain (NdotV)(NdotL), but they'd be
	// canceled out by our G() term.  As such, they have been removed
	// from BOTH places to prevent floating point rounding errors.
    float3 specularResult = (D * F * G) / 4;

	// One last non-obvious requirement: According to the rendering equation,
	// specular must have the same NdotL applied as diffuse!  We'll apply
	// that here so that minimal changes are required elsewhere.
    return specularResult * max(dot(n, l), 0);
}



#endif
