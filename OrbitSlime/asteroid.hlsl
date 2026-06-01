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

float4 PS(PS_IN input) : SV_Target
{
    float3 p = input.worldPos;
    float3 n = normalize(input.normal);
    float3 l = normalize(lightDir);
    float3 v = normalize(cameraPos - input.worldPos);
    
    float3 planetCenter = float3(0.0f, 0.0f, 0.0f);
    float3 dirToPlanet = normalize(planetCenter - input.worldPos);
    float front = saturate(dot(n, dirToPlanet));
    
    float3 fireColor = float3(1.0f, 0.35f, 0.05f);
    float fireStrength = 3.0f;
    float3 fire = fireColor * fireStrength * pow(front, 1.5f);
    
    
    
    float ndotl = saturate(dot(n, l));
    float3 reflection = reflect(-l, n);
    float specular = 0.0f;
    if (ndotl > 0.0f)
    {
        specular = pow(saturate(dot(reflection, v)), specularPower) * specularStrength;
    }
    
    float3 ambientColor = tintColor.rgb * ambient;
    float3 diffuseColor = tintColor.rgb * lightColor * ndotl * diffuseStrength;
    float3 specularColor = lightColor * specular;
    float3 finalColor = ambientColor + diffuseColor + specularColor + fire;
    
    return float4(finalColor, tintColor.a);
}
