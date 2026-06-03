cbuffer cbWorld : register(b0)
{
    matrix matWorld;
};

cbuffer cbMaterial : register(b1)
{
    float4 tintColor;
    float specularStrength;
    float specularPower;
    float2 materialPadding;
};

cbuffer cbLight : register(b2)
{
    float3 lightDir;
    float ambient;
    float3 lightColor;
    float diffuseStrength;
};

cbuffer cbCamera : register(b3)
{
    matrix matView;
    matrix matProjection;
    float3 cameraPos;
    float cameraPadding;
};

#define MAX_POINT_LIGHTS 16
struct PointLight
{
    float3 position;
    float radius;
    float3 color;
    float intensity;
};

cbuffer cbPointLight : register(b4)
{
    PointLight pointLights[MAX_POINT_LIGHTS];
    int pointLightCount;
    float3 pointLightPadding;
};

struct VS_IN
{
    float3 pos : POSITION;
    float4 col : COLOR;
    float3 normal : NORMAL;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float3 normal : NORMAL;
    float3 worldPos : POSITION1;
};

PS_IN VS(VS_IN input)
{
    PS_IN output;
    float4 worldPos = mul(float4(input.pos, 1.0f), matWorld);
    float4 viewPos = mul(worldPos, matView);
    output.pos = mul(viewPos, matProjection);
    output.col = input.col;
    output.normal = mul(float4(input.normal, 0.0f), matWorld).xyz;
    output.worldPos = worldPos.xyz;
    return output;
}

float CalculateSpecular(float3 normal, float3 lightDirection, float3 viewDirection, float ndotl)
{
    if (ndotl <= 0.0f)
    {
        return 0.0f;
    }

    float3 reflection = reflect(-lightDirection, normal);
    return pow(saturate(dot(reflection, viewDirection)), specularPower) * specularStrength;
}

float3 CalculatePointLight(PointLight light, float3 worldPos, float3 normal, float3 viewDirection)
{
    float3 toLight = light.position - worldPos;
    float distanceToLight = length(toLight);

    if (distanceToLight > light.radius)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }

    float3 pointLightDir = normalize(toLight);
    float attenuation = saturate(1.0f - distanceToLight / light.radius);
    attenuation *= attenuation;

    float ndotl = saturate(dot(normal, pointLightDir));
    float3 diffuseColor = tintColor.rgb * light.color * ndotl * light.intensity * attenuation;

    float specular = CalculateSpecular(normal, pointLightDir, viewDirection, ndotl);
    float3 specularColor = light.color * specular * light.intensity * attenuation;

    return diffuseColor + specularColor;
}

float3 CalculatePointLights(float3 worldPos, float3 normal, float3 viewDirection)
{
    float3 result = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < pointLightCount; i++)
    {
        result += CalculatePointLight(pointLights[i], worldPos, normal, viewDirection);
    }

    return result;
}

float4 PS(PS_IN input) : SV_Target
{
    float3 n = normalize(input.normal);
    float3 l = normalize(lightDir);
    float3 v = normalize(cameraPos - input.worldPos);

    float ndotl = saturate(dot(n, l));
    float specular = CalculateSpecular(n, l, v, ndotl);

    float3 ambientColor = tintColor.rgb * ambient;
    float3 diffuseColor = tintColor.rgb * lightColor * ndotl * diffuseStrength;
    float3 specularColor = lightColor * specular;
    float3 finalColor = ambientColor + diffuseColor + specularColor;
    finalColor += CalculatePointLights(input.worldPos, n, v);

    return float4(finalColor, tintColor.a);
}
