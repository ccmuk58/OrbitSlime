cbuffer cbWorld : register(b0)
{
    matrix matWorld;
};

cbuffer cbCamera : register(b3)
{
    matrix matView;
    matrix matProjection;
    float3 cameraPos;
    float cameraPadding;
};

Texture2D digitTexture : register(t0);
SamplerState digitSampler : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float4 col : COLOR;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 uv : TEXCOORD;
};

PS_IN VS(VS_IN input)
{
    PS_IN output;

    
    float4 worldPos = mul(float4(input.pos, 1.0f), matWorld);
    float4 viewPos = mul(worldPos, matView);
    output.pos = mul(viewPos, matProjection);
    output.col = input.col;
    output.uv = input.uv;
    return output;
}

float4 PS(PS_IN input) : SV_Target
{
    float4 sampled = digitTexture.Sample(digitSampler, input.uv) * input.col;
    float brightness = max(sampled.r, max(sampled.g, sampled.b));

    
    
    sampled.a = sampled.a * smoothstep(0.35f, 0.75f, brightness);

    
    sampled.rgb = float3(1.0f, 1.0f, 1.0f);
    return sampled;
}
