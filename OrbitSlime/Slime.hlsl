cbuffer cbWorld : register(b0)
{
    matrix matWorld;
};

cbuffer cbMaterial : register(b1)
{
    float4 tintColor;
};
cbuffer cbLight : register(b2)
{
    float3 lightDir;
    float ambient;
    float3 lightColor;
    float diffuseStrength;
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
};

PS_IN VS(VS_IN input)
{
    PS_IN output;
    output.pos = mul(float4(input.pos, 1.0f), matWorld);
    output.col = input.col;
    output.normal = mul(float4(input.normal, 0.0f), matWorld).xyz;
    return output;
}

float4 PS(PS_IN input) : SV_Target
{
    float3 n = normalize(input.normal);
    float3 l = normalize(lightDir);
    
    float ndotl = saturate(dot(n, l));
    float lighting = ambient + ndotl * diffuseStrength;
    
    float3 finalColor = tintColor.rgb * lightColor * lighting;
    
    return float4(finalColor, tintColor.a);
}
