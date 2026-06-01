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

float CalculatePlanetShadow(float3 worldPos, float3 lightDirection)
{
    float3 planetCenter = float3(0.0f, 0.0f, 0.0f);
    float planetRadius = 0.3f;
    float shadowSoftness = 0.08f;

    float3 toPlanet = planetCenter - worldPos;
    float projected = dot(toPlanet, lightDirection);

    if (projected <= 0.0f)
    {
        return 0.0f;
    }

    float3 closestPoint = worldPos + lightDirection * projected;
    float distToRay = length(planetCenter - closestPoint);

    return 1.0f - smoothstep(planetRadius, planetRadius + shadowSoftness, distToRay);
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

float3 CalculatePhongLighting(float3 normal, float3 lightDirection, float3 viewDirection)
{
    float ndotl = saturate(dot(normal, lightDirection));
    float specular = CalculateSpecular(normal, lightDirection, viewDirection, ndotl);

    float3 ambientColor = tintColor.rgb * ambient;
    float3 diffuseColor = tintColor.rgb * lightColor * ndotl * diffuseStrength;
    float3 specularColor = lightColor * specular;

    return ambientColor + diffuseColor + specularColor;
}

float3 ApplyPlanetShadow(float3 litColor, float3 ambientColor, float shadow)
{
    float shadowStrength = 0.65f;
    float shadowFactor = 1.0f - shadow * shadowStrength;

    return ambientColor + (litColor - ambientColor) * shadowFactor;
}

float4 PS(PS_IN input) : SV_Target
{

    float3 n = normalize(input.normal);
    float3 l = normalize(lightDir);
    float3 v = normalize(cameraPos - input.worldPos);

    float3 ambientColor = tintColor.rgb * ambient;
    float3 litColor = CalculatePhongLighting(n, l, v);
    float shadow = CalculatePlanetShadow(input.worldPos, l);
    float3 finalColor = ApplyPlanetShadow(litColor, ambientColor, shadow);

    return float4(finalColor, tintColor.a);
}
